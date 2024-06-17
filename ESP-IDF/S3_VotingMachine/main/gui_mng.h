/*
 * gui_mng.h
 *
 *  Created on: Mar 2, 2024
 *      Author: xpress_embedo
 */

#ifndef MAIN_GUI_MNG_H_
#define MAIN_GUI_MNG_H_

// Include Header Files
#include "freertos/FreeRTOS.h"
#include "gui_mng_cfg.h"

typedef struct _gui_q_msg_t {
  gui_mng_event_t   event_id;
  uint8_t           *data;
} gui_q_msg_t;

// Public Function Prototypes
void gui_start( void );
BaseType_t gui_send_event( gui_mng_event_t event, uint8_t *pData );
uint8_t gui_update_lock( void );
void gui_update_unlock( void );



#endif /* MAIN_GUI_MNG_H_ */
