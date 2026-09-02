/*
 * gui_mng.h
 *
 *  Created on: Mar 2, 2024
 *      Author: xpress_embedo
 */

#ifndef MAIN_GUI_MNG_H_
#define MAIN_GUI_MNG_H_

// Include Header Files
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "gui_mng_cfg.h"

typedef struct _gui_q_msg_t {
  gui_mng_event_t   event_id;
  // Local typed copy of event data owned by this queue item.
  gui_mng_event_data_t data;
} gui_q_msg_t;

// Public Function Prototypes
bool gui_start( void );
BaseType_t gui_send_event( gui_mng_event_t event, const gui_mng_event_data_t *pData );
uint8_t gui_update_lock( void );
void gui_update_unlock( void );

#endif /* MAIN_GUI_MNG_H_ */
