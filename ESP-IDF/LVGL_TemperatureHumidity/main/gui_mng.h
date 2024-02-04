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

// Public Function Prototypes
void gui_start( void );
void gui_set_event( gui_mng_event_t event );
uint8_t gui_update_lock( void );
void gui_update_unlock( void );

#endif /* MAIN_GUI_MNG_H_ */
