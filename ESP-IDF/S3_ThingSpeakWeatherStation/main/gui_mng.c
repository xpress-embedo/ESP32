/*
 * gui_mng.c
 *
 *  Created on: Mar 2, 2024
 *      Author: xpress_embedo
 */
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "time.h"
#include "lvgl.h"
#include "gui_mng.h"
#include "lcd.h"

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


// Public Function Definition
/**
 * @brief GUI Start Function, this function will start the gui manager task
 * @param  none
 */
void gui_start( void )
{
  gui_init();

  // callback function, task name, stack size, parameters, priority, task handle
  xTaskCreatePinnedToCore(&gui_task, "gui task", 4096*2, NULL, 5, NULL, 0);
  // NOTE: I checked the flush timing with pinning and without pinning to core is same
}

/**
 * @brief Send GUI Event
 * @param event Event Code
 * @param pData Pointer to Data if Any
 * @return BaseType_t pdTRUE if successful else pdFALSE
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
  lcd_init();
  // turn on the backlight
  lcd_set_backlight(true);

  // main user interface
  // gui_cfg_init();
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
        // todo
        // gui_cfg_mng_process(msg.event_id, msg.data);
      }   // if event received in limit end
    }     // xQueueReceive end
  }
}

/**
 * @brief gui refresh, this function will refresh the lvgl
 * @param  none
 */
static void gui_refresh( void )
{
  if( GUI_LOCK() )
  {
    lv_timer_handler();
    // Semaphore is released when flushing is completed, this is checked using
    // tft_flush_status function, and then we release the semaphore
    GUI_UNLOCK();
  }
}
