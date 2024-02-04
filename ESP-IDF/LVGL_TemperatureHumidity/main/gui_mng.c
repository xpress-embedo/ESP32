/*
 * gui_mng.c
 *
 *  Created on: Feb 4, 2024
 *      Author: xpress_embedo
 */
#include "esp_timer.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "ui.h"
#include "main.h"
#include "lvgl.h"
#include "gui_mng.h"
#include "display_mng.h"

// Macros
#define GUI_LOCK()                        gui_update_lock()
#define GUI_UNLOCK()                      gui_update_unlock()
#define GUI_EVENT_QUEUE_LEN               (5)

// Private Variables
static const char *TAG = "GUI";
// Creates a semaphore to handle concurrent call to lvgl stuff, If we wish to
// call *any* lvgl function from other threads/tasks we should lock on the very
// same semaphore!
static SemaphoreHandle_t  gui_semaphore;
static QueueHandle_t      gui_event;
static lv_chart_series_t * temp_series;
static lv_chart_series_t * humid_series;

// Private Function Declaration
static void gui_init( void );
static void gui_task(void *pvParameter);
static void gui_refresh( void );
static void gui_update_temp_humid( void );

// Public Function Definition

/**
 * @brief GUI Start Function, this function will start the gui manager task
 * @param  none
 */
void gui_start( void )
{
  gui_init();

  // callback function, task name, stack size, parameters, priority, task handle
  // xTaskCreate(&gui_task, "gui task", 4096*4, NULL, 5, NULL);
  xTaskCreatePinnedToCore(&gui_task, "gui task", 4096*4, NULL, 5, NULL, 0);
  // NOTE: I checked the flush timing with pinning and without pinning to core is same
}

BaseType_t gui_send_event( gui_mng_event_t event, uint8_t *pData )
{
  BaseType_t status = pdFALSE;
  gui_q_msg_t msg;

  if( event < GUI_MNG_EV_MAX )
  {
    msg.event_id  = event;
    // msg.data      = pData;
    status = xQueueSend( gui_event, &msg, portMAX_DELAY );
  }
  return status;
}

/**
 * @brief Lock the display update with a semaphore
 *        Creates a semaphore to handle concurrent call to lvgl stuff
 * @param   none
 * @return  if locking is successful else false
 * @note    Check this link https://docs.lvgl.io/8.3/porting/os.html#
 */
uint8_t gui_update_lock( void )
{
  uint8_t status = false;
  if( pdTRUE == xSemaphoreTake(gui_semaphore, portMAX_DELAY) )
  {
    status = true;
  }
  return status;
}

/**
 * @brief Unlock the display update from a semaphore
 * @param  none
 */
void gui_update_unlock( void )
{
  xSemaphoreGive(gui_semaphore);
}


// Private Function Definitions

/**
 * @brief gui initialization task, this will initialize the semaphire and display
 * @param  none
 */
static void gui_init( void )
{
  gui_semaphore = xSemaphoreCreateMutex();

  // initialize display related stuff, also lvgl
  display_init();

  // main user interface
  ui_init();

  // Below are some updates for chart related handling
  sensor_data_t *sensor_data = get_temperature_humidity();
  uint8_t *temp_data = sensor_data->temperature;
  uint8_t *humid_data = sensor_data->humidity;

  // this should match with the temperature & humidity buffer length
  // NOTE: this is also configured in Square Line Studio as 100, so must match
  uint16_t chart_hor_res = SENSOR_BUFF_SIZE;
  // By default the number of points are 10, update it to chart width
  // this is already updated in Square Line Studio as 100
  lv_chart_set_point_count( ui_chart, chart_hor_res );

  // Do not display points on the data
  lv_obj_set_style_size( ui_chart, 0, LV_PART_INDICATOR);

  // Update mode shift or circular, here shift is selected
  lv_chart_set_update_mode( ui_chart, LV_CHART_UPDATE_MODE_SHIFT );

  // Add data series for temperature on primary y-axis
  temp_series = lv_chart_add_series(ui_chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);
  // Add data series for humidity on secondary y-axis
  humid_series = lv_chart_add_series(ui_chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_SECONDARY_Y);

  for( size_t idx=0; idx<chart_hor_res; idx++ )
  {
    temp_series->y_points[idx] = (lv_coord_t)*(temp_data+idx);
    humid_series->y_points[idx] = (lv_coord_t)*(humid_data+idx);
  }
}

/**
 * @brief gui task Function which calls the lvgl timer handler function
 *        and other updates on the user interface based on the events received
 * @param *pvParameter  task parameter
 */
static void gui_task(void *pvParameter)
{
  gui_q_msg_t msg;
  msg.event_id = GUI_MNG_EV_NONE;

  // create message queue with the length GUI_EVENT_QUEUE_LEN
  gui_event = xQueueCreate( GUI_EVENT_QUEUE_LEN, sizeof(gui_q_msg_t) );
  if( gui_event )
  {
    ESP_LOGE(TAG, "Unable to Create Queue");
  }

  while(1)
  {
    vTaskDelay(pdMS_TO_TICKS(20));

    // refresh the display
    gui_refresh();

    // wait only 5 ms and then proceed
    if( xQueueReceive(gui_event, &msg, pdMS_TO_TICKS(5)) )
    {
      // the below is the code to handle the state machine
      if( GUI_MNG_EV_NONE != msg.event_id )
      {
        switch( msg.event_id )
        {
          case GUI_MNG_EV_TEMP_HUMID:
            gui_update_temp_humid();
            gui_event = GUI_MNG_EV_NONE;
            break;
          default:
            gui_event = GUI_MNG_EV_NONE;
            break;
        } // switch case end
      }   // if event received in limit end
    }     // xQueueReceive end
  }
}

/**
 * @brief gui refresh, this function will refresh the lvgl
 * @param  none
 */
static int max_flushing_time = 0;
static void gui_refresh( void )
{
  int64_t start_time = 0;
  if( GUI_LOCK() )
  {
    start_time = esp_timer_get_time();
    lv_timer_handler();
    // Semaphore is released when flushing is completed, this is checked using
    // tft_flush_status function, and then we release the semaphore
    // GUI_UNLOCK();
  }

  // check flushing status
  if( tft_flush_status() == true )
  {
    // printf("Flushing Time: %d" PRId64 ", %" PRId64 "\n", esp_timer_get_time(), start_time);
    int time_taken = (int32_t)((esp_timer_get_time() - start_time)/1000);
    if( time_taken > max_flushing_time )
    {
      max_flushing_time = time_taken;
      printf("Flushing Time: %d ms\n", max_flushing_time );
    }
    GUI_UNLOCK();
  }
}

/**
 * @brief Update temperature and humidity data
 * @param  none
 */
static void gui_update_temp_humid( void )
{
  /* NOTE: this function is also called inside the same task where we are calling
   * lv_timer_handler function, hence we don't need to use any mutex lock here
   * Check this link: https://docs.lvgl.io/8.3/porting/os.html#
   */
  // if( GUI_LOCK() )
  {
    sensor_data_t *sensor_data = get_temperature_humidity();
    size_t idx = sensor_data->sensor_idx;
    if( (idx > 0) && (idx < SENSOR_BUFF_SIZE) )
    {
      // before posting the event we have incremented the index and hence to get
      // the last sensor data we have to use - 1
      idx = (idx - 1);
      lv_label_set_text_fmt(ui_lblTemperatureValue, "%d °C", sensor_data->temperature[idx] );
      lv_label_set_text_fmt(ui_lblHumidityValue, "%d %%", sensor_data->humidity[idx] );
    }

    // update chart
    uint8_t *temperature_data = sensor_data->temperature;
    uint8_t *humidity_data    = sensor_data->humidity;
    // this should match the temperature buffer length
    uint16_t chart_hor_res = SENSOR_BUFF_SIZE;

    for( idx=0; idx<chart_hor_res; idx++ )
    {
      temp_series->y_points[idx] = (lv_coord_t)*(temperature_data+idx);
      humid_series->y_points[idx] = (lv_coord_t)*(humidity_data+idx);
    }
    lv_chart_refresh(ui_chart);
    // GUI_UNLOCK();
  }
}
