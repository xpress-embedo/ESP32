/*
 * gui_mng_cfg.h
 *
 *  Created on: Mar 2, 2024
 *      Author: xpress_embedo
 */

#ifndef MAIN_GUI_MNG_CFG_H_
#define MAIN_GUI_MNG_CFG_H_

#include <stdint.h>

// Public Macros
#define GUI_MNG_REFRESH_TIME            (5)   // in milliseconds

// Enumeration
typedef enum {
  GUI_MNG_EV_NONE = 0,
  GUI_MNG_EV_STARTUP,             		// Event for GUI startup
  GUI_MNG_EV_IR_COMMAND,              // Event for IR command received
  GUI_MNG_EV_MAX,
} gui_mng_event_t;

/*
 * Typed GUI event payload.
 *
 * Idea:
 * Instead of sending "some bytes" and remembering what they mean,
 * we define named payload shapes here.
 *
 * For example, if later you add an event like "show temperature",
 * you can add a new struct member in this union and use that member directly.
 */
typedef union {
  struct {
    uint32_t reserved;
  } startup;

  struct {
    uint32_t value;
  } number;

  struct {
    uint16_t x;
    uint16_t y;
  } point;

  struct {
    // Maximum text length of the command is 22 bytes
    char command_msg[25];
  } infrared;
} gui_mng_event_data_t;


// Public Function Prototypes
void gui_cfg_init( void );
void gui_cfg_mng_process( gui_mng_event_t event, const gui_mng_event_data_t *data );
void gui_cfg_refresh( void );

#endif /* MAIN_GUI_MNG_CFG_H_ */
