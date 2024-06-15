/*
 * gui_mng_cfg.h
 *
 *  Created on: Mar 2, 2024
 *      Author: xpress_embedo
 */

#ifndef MAIN_GUI_MNG_CFG_H_
#define MAIN_GUI_MNG_CFG_H_

typedef enum {
  GUI_MNG_EV_NONE = 0,
  GUI_MNG_EV_TEMP_HUMID,          // Event for temperature and humidity update
  GUI_MNG_EV_MAX,
} gui_mng_event_t;


// Public Function Prototypes
void gui_cfg_init( void );
void gui_cfg_mng_process( gui_mng_event_t event, uint8_t *data );


#endif /* MAIN_GUI_MNG_CFG_H_ */
