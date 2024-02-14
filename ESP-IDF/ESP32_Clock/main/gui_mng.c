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

#include "time.h"
#include "ui.h"
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
static QueueHandle_t      gui_event = NULL;

// Private Function Declaration
static void gui_init( void );
static void gui_task(void *pvParameter);
static void gui_refresh( void );
static void gui_update_time( uint8_t *pData );
static void gui_display_sntp_connecting( void );
static void gui_load_clock_screen( void );

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
  xTaskCreatePinnedToCore(&gui_task, "gui task", 4096*2, NULL, 5, NULL, 0);
  // NOTE: I checked the flush timing with pinning and without pinning to core is same
}

/**
 * @brief Send GUI Event
 * @param event Event Code
 * @param pData Pointer to Data if Any
 * @return BaseType_t pdTRUE if successfull else pdFALSE
 */
BaseType_t gui_send_event( gui_mng_event_t event, uint8_t *pData )
{
  BaseType_t status = pdFALSE;
  gui_q_msg_t msg;

  if( event < GUI_MNG_EV_MAX )
  {
    msg.event_id  = event;
    msg.data      = pData;
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

  // create message queue with the length GUI_EVENT_QUEUE_LEN
  gui_event = xQueueCreate( GUI_EVENT_QUEUE_LEN, sizeof(gui_q_msg_t) );
  if( gui_event == NULL )
  {
    ESP_LOGE(TAG, "Unable to Create Queue");
  }

  // initialize display related stuff, also lvgl
  display_init();

  // main user interface
  ui_init();
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

  while(1)
  {
    vTaskDelay(pdMS_TO_TICKS(20));

    // refresh the display
    gui_refresh();

    // wait only 5 ms and then proceed
    if( xQueueReceive(gui_event, &msg, pdMS_TO_TICKS(10)) )
    {
      // the below is the code to handle the state machine
      if( GUI_MNG_EV_NONE != msg.event_id )
      {
        switch( msg.event_id )
        {
          case GUI_MNG_EV_TEMP_HUMID:
            break;
          case GUI_MNG_EV_TIME_UPDATE:
            gui_update_time( msg.data );
            break;
          case GUI_MNG_EV_WIFI_CONNECTED:
            gui_display_sntp_connecting();
            break;
          case GUI_MNG_EV_SNTP_SYNC:
            gui_load_clock_screen();
            break;
          default:
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
 * @brief Update the Time of the display, this function is called whenever we
 *        receive GUI_MNG_EV_TIME_UPDATE event, the pointer to time information
 *        is passed in the message queue, which is used to fetch the time info
 *        directly, otherwise we have to use some helper function to get this.
 * @param pData pointer to time information
 * @note  Need to investigate if usage of pointer is safe or not, because we are
 *        updating the time every 1 second.
 */
static void gui_update_time( uint8_t *pData )
{
  struct tm *time_info;
  time_info = (struct tm*)pData;
  int16_t seconds_angle = (int16_t)(time_info->tm_sec *60);
  int16_t minute_angle = (int16_t)(time_info->tm_min *60);
  int16_t hour_angle = (int16_t)(time_info->tm_hour*5 *60);
  lv_img_set_angle(ui_imgSecond, seconds_angle);
  lv_img_set_angle(ui_imgSecDot, seconds_angle);
  lv_img_set_angle(ui_imgMinute, minute_angle);
  lv_img_set_angle(ui_imgHour, hour_angle);
  // the below commented part is the simple method without using the pointer
  // int16_t sec_angle = get_seconds();   // this helper function is needed.
  // int16_t sec_angle = pData
  // lv_img_set_angle(ui_imgSecond, sec_angle*60);
}

static void gui_display_sntp_connecting( void )
{
  lv_label_set_text(ui_lblConnecting, "Synchronizing with NTP...");
}

/**
 * @brief Load the analog clock screen.
 */
static void gui_load_clock_screen( void )
{
  lv_disp_load_scr(ui_ClockScreen);
}
