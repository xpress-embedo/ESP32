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

// Macros
#define GUI_LOCK()                        gui_update_lock()
#define GUI_UNLOCK()                      gui_update_unlock()
#define GUI_EVENT_QUEUE_LEN               (5)

// Private Variables
static const char *TAG = "GUI";
// LVGL is not thread-safe by default.
// If more than one task can call LVGL functions, all of them must use the same
// lock, otherwise one task can modify UI objects while another task is drawing.
static SemaphoreHandle_t  gui_semaphore;
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
  if( !gui_init() )
  {
    return false;
  }

  // callback function, task name, stack size, parameters, priority, task handle
  if( pdPASS != xTaskCreate(
    &gui_task, 
    "gui task", 
    8192,
    NULL,
    4,
    NULL) )
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
  if( (gui_semaphore != NULL) && (pdTRUE == xSemaphoreTake(gui_semaphore, portMAX_DELAY)) )
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
  if( gui_semaphore != NULL )
  {
    xSemaphoreGive(gui_semaphore);
  }
}

// Private Function Definitions

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
  gui_semaphore = xSemaphoreCreateMutex();
  if( gui_semaphore == NULL )
  {
    ESP_LOGE(TAG, "Unable to create GUI mutex");
    return false;
  }

  // create message queue with the length GUI_EVENT_QUEUE_LEN
  gui_event = xQueueCreate( GUI_EVENT_QUEUE_LEN, sizeof(gui_q_msg_t) );
  if( gui_event == NULL )
  {
    ESP_LOGE(TAG, "Unable to Create Queue");
    vSemaphoreDelete(gui_semaphore);
    gui_semaphore = NULL;
    return false;
  }

  // initialize display related stuff, also lvgl
  lcd_init();
  // turn on the backlight
  lcd_set_backlight(true);

  // main user interface
  gui_cfg_init();

  // Build the first screen from inside the GUI module so startup is self-contained.
  // gui_cfg_mng_process( GUI_MNG_EV_STARTUP, NULL );

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
static void gui_task(void *pvParameter)
{
  gui_q_msg_t msg;
  msg.event_id = GUI_MNG_EV_NONE;

  while(1)
  {
    // TODO: need to understand, mostly we don't have data in queue, so there is
    // already a delay, I think we don't need this delay
    // vTaskDelay(pdMS_TO_TICKS(20));

    // refresh the display
    gui_refresh();
    
    // custom configurable function
    gui_cfg_refresh();

    // wait only GUI_MNG_REFRESH_TIME ms and then proceed
    if( xQueueReceive(gui_event, &msg, pdMS_TO_TICKS(GUI_MNG_REFRESH_TIME)) )
    {
      // the below is the code to handle the state machine
      if( GUI_MNG_EV_NONE != msg.event_id )
      {
        gui_cfg_mng_process(msg.event_id, &msg.data);
      }   // if event received in limit end
    }     // xQueueReceive end
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
  if( GUI_LOCK() )
  {
    // LVGL engine: processes animations, input, and redraw scheduling.
    // Call this periodically from a FreeRTOS task.
    lcd_lvgl_timer_handler();
    // Semaphore is released when flushing is completed, this is checked using
    // tft_flush_status function, and then we release the semaphore
    GUI_UNLOCK();
  }
}
