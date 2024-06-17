/*
 * gui_mng_cfg.c
 *
 *  Created on: Jun 15, 2024
 *      Author: xpress_embedo
 */

#include "main.h"
#include "lvgl.h"
#include "gui_mng.h"
#include "gui_mng_cfg.h"

#include "ui.h"

// Private Macros
#define NUM_ELEMENTS(x)                 (sizeof(x)/sizeof(x[0]))

// function template for callback function
typedef void (*gui_mng_callback)(uint8_t * data);

typedef struct _gui_mng_event_cb_t
{
  gui_mng_event_t   event;
  gui_mng_callback  callback;
} gui_mng_event_cb_t;

// Private Function Prototypes


// Private Variables

//static const gui_mng_event_cb_t gui_mng_event_cb[] =
//{
//};

// Public Function Definitions

/**
 * @brief GUI Configurable Initialization Function
 * @param  None
 */
void gui_cfg_init( void )
{
  ui_init();
}

/**
 * @brief Process the events posted to GUI manager module
 *        This function calls the dedicated function based on the event posted
 *        to GUI manager queue, I will think of moving this function to GUI manager
 * @param event event name
 * @param data event data pointer
 */
void gui_cfg_mng_process( gui_mng_event_t event, uint8_t *data )
{
//  uint8_t idx = 0;
//  for( idx=0; idx < NUM_ELEMENTS(gui_mng_event_cb); idx++ )
//  {
//    // check if event matches the table
//    if( event == gui_mng_event_cb[idx].event )
//    {
//      // call the callback function with arguments, if not NULL
//      if( gui_mng_event_cb[idx].callback != NULL )
//      {
//        gui_mng_event_cb[idx].callback(data);
//      }
//    }
//  }
}

// Private Function Definitions

