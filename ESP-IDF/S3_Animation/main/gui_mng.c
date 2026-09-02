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

#include "gui_mng.h"
#include "lcd.h"

/* Private Macros */
/* IMPORTANT NOTE 
In my previous projects based on ESP32 controller, I used to create a semaphore 
for LVGL tasks, because LVGL is not thread safe, so it must be protected when
multiple tasks tried to update the display buffer by calling the LVGL function.
Here it is not required because in lv_conf.h file, I have enabled the 
LV_USE_OS macro to use the FreeRTOS kernel for LVGL tasks.
#define LV_USE_OS   LV_OS_FREERTOS
so we can use lv_lock() and lv_unlock() function instead of the one I created 
earlier on ESP32. The mutex created by LVGL is Recursive Mutex xSemaphoreCreateRecursiveMutex
which means that a task can acquire the mutex multiple times without blocking 
itself, and release it when it is no longer needed. I mentioned this because 
lv_timer_handler() has already protected by this locking and unlocking, but even 
if I use it again, there will be no impact, due to the usage of recursive mutex.
*/
#define GUI_EVENT_QUEUE_LEN               (5)

// Private Variables
static const char *TAG = "GUI";
// Queue used to move GUI requests from other tasks into the dedicated GUI task.
static QueueHandle_t      gui_event = NULL;

// Private Function Declaration
static bool gui_init( void );
static void gui_task(void *pvParameter);
static void gui_refresh( void );


// Public Function Definition
/**
 * @brief GUI Start Function, this function will start the gui manager task
 * @param  none
 * @return true if GUI manager started successfully, else false
 */
bool gui_start( void )
{
  /* callback function, task name, stack size, parameters, priority, task handle */
  if( pdPASS != xTaskCreate( &gui_task, "gui task", 8192, NULL, 4, NULL) )
  {
    ESP_LOGE(TAG, "Unable to create GUI task");
    return false;
  }

  return true;
}

/**
 * @brief Send GUI Event
 *
 * Important design idea:
 * We copy one typed payload into the queue item instead of storing only a
 * pointer. This is safer because the sender's original variable may be local
 * to another function and may disappear before the GUI task reads the event.
 *
 * Example of unsafe pointer usage:
 * 1) task A creates a local variable
 * 2) task A sends pointer to queue
 * 3) task A returns from function or changes that variable
 * 4) GUI task reads old pointer later
 * 5) pointer still has an address, but data at that address is now wrong
 *
 * By copying a typed payload into the queue message, the GUI task always
 * receives its own valid snapshot of the data.
 *
 * @param event Event code.
 * @param pData Pointer to typed source data to copy, can be NULL.
 * @return BaseType_t pdTRUE if successful else pdFALSE
 */
BaseType_t gui_send_event( gui_mng_event_t event, const gui_mng_event_data_t *pData )
{
  BaseType_t status = pdFALSE;
  gui_q_msg_t msg = {0};

  if( (event < GUI_MNG_EV_MAX) && (gui_event != NULL) )
  {
    msg.event_id  = event;

    // Copy payload now so later queue processing does not depend on caller's
    // variable lifetime.
    if( pData != NULL )
    {
      msg.data = *pData;
    }

    status = xQueueSend( gui_event, &msg, portMAX_DELAY );
  }
  return status;
}

/* Private Function Definitions */

/**
 * @brief Initialize GUI manager resources and the first screen.
 *
 * This function creates:
 * 1) LVGL lock (mutex)
 * 2) GUI event queue
 * 3) LCD/LVGL/touch stack
 * 4) initial UI screen
 *
 * Startup screen creation is done here so the GUI module is self-contained.
 * The caller only needs to start GUI once and does not need to remember to
 * send a separate startup event afterward.
 * @param  none
 * @return true if initialization completed successfully, else false
 */
static bool gui_init( void )
{
  /* create message queue with the length GUI_EVENT_QUEUE_LEN */
  gui_event = xQueueCreate( GUI_EVENT_QUEUE_LEN, sizeof(gui_q_msg_t) );
  if( gui_event == NULL )
  {
    ESP_LOGE(TAG, "Unable to Create Queue");
    return false;
  }

  /* Initialize the LVGL library and TFT display driver */
  lcd_init();
  /* turn on the backlight */
  lcd_set_backlight(true);

  return true;
}

/**
 * @brief gui task Function which calls the lvgl timer handler function
 *        and other updates on the user interface based on the events received
 *
 * This task acts like a "GUI owner" task.
 * Other tasks should send requests/events here instead of directly changing UI
 * objects whenever possible.
 * That keeps UI updates organized in one place.
 *
 * @param *pvParameter  task parameter
 */
static void gui_task( void *pvParameters )
{
  gui_q_msg_t msg;
  msg.event_id = GUI_MNG_EV_NONE;

  /* Initialize LVGL and Drivers */
  if ( gui_init() )
  {
    /* main user interface */
    gui_cfg_init();

    /* send an event to display the start-up screen */
    gui_send_event( GUI_MNG_EV_STARTUP , NULL );
  }
  
  while (1)
  {
    /* Refresh the display */
    gui_refresh();

    /* custom configurable function */
    gui_cfg_refresh();

    /* wait only for GUI_MNG_REFRESH_TIME in ms and then proceed */
    if ( xQueueReceive( gui_event, &msg, pdMS_TO_TICKS(GUI_MNG_REFRESH_TIME) ) )
    {
      // the below is the code to handle the state machine
      if( GUI_MNG_EV_NONE != msg.event_id )
      {
        gui_cfg_mng_process(msg.event_id, &msg.data);
      }   // if event received in limit end
    }
  }
}

/**
 * @brief gui refresh, this function will refresh the lvgl
 * This function should be called periodically from a task context.
 * It drives LVGL internals (redraw, animation, input processing).
 * @param  none
 */
static void gui_refresh( void )
{
  /* Handle GUI events and updates */
  lcd_lvgl_timer_handler();
}
