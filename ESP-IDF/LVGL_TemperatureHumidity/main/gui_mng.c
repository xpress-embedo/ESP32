/*
 * gui_mng.c
 *
 *  Created on: Feb 4, 2024
 *      Author: xpress_embedo
 */
#include "esp_timer.h"

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

// Private Variables
// Creates a semaphore to handle concurrent call to lvgl stuff, If we wish to
// call *any* lvgl function from other threads/tasks we should lock on the very
// same semaphore!
static SemaphoreHandle_t gui_semaphore;
static gui_mng_event_t gui_event = GUI_MNG_EV_NONE;

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

void gui_set_event( gui_mng_event_t event )
{
  if( event < GUI_MNG_EV_MAX )
  {
    gui_event = event;
  }
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
}

/**
 * @brief gui task Function which calls the lvgl timer handler function
 *        and other updates on the user interface based on the events received
 * @param *pvParameter  task parameter
 */
static void gui_task(void *pvParameter)
{
  while(1)
  {
    vTaskDelay(pdMS_TO_TICKS(20));

    gui_refresh();

    // the below is the code to handle the state machine
    if( GUI_MNG_EV_NONE != gui_event )
    {
      switch( gui_event )
      {
        case GUI_MNG_EV_TEMP_HUMID:
          gui_update_temp_humid();
          gui_event = GUI_MNG_EV_NONE;
          break;
        default:
          gui_event = GUI_MNG_EV_NONE;
          break;
      }
    }
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
    lv_label_set_text_fmt(ui_lblTemperatureValue, "%d °C", get_temerature());
    lv_label_set_text_fmt(ui_lblHumidityValue, "%d %%", get_humidity());
    // ESP_LOGI(TAG, "Variable Temperature = %d, Humidity = %d Updated on Display", temperature, humidity);
    // ESP_LOGI(TAG, "Label => Temperature = %s, Humidity = %s", lv_label_get_text(ui_lblTemperatureValue), lv_label_get_text(ui_lblHumidityValue));
    // GUI_UNLOCK();
  }
}
