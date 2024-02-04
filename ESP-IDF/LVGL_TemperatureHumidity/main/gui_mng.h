/*
 * gui_mng.h
 *
 *  Created on: Feb 4, 2024
 *      Author: xpress_embedo
 */

#ifndef MAIN_GUI_MNG_H_
#define MAIN_GUI_MNG_H_

// Include Header Files
// todo: maybe in future

typedef enum {
  GUI_MNG_EV_NONE = 0,
  GUI_MNG_EV_TEMP_HUMID = 1,    // Event for temperature and humidity update
  GUI_MNG_EV_MAX,
} gui_mng_event_t;

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
