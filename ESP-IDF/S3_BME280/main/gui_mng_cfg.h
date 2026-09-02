/*
 * gui_mng_cfg.h
 *
 *  Created on: Mar 2, 2024
 *      Author: abc@xyz
 */

#ifndef GUI_MNG_CFG_H_
#define GUI_MNG_CFG_H_

#include <stdint.h>

/* Private Macros */
#define GUI_MNG_REFRESH_TIME            (5)   // in milliseconds

// Enumeration
typedef enum {
  GUI_MNG_EV_NONE = 0,
  GUI_MNG_EV_STARTUP,             		  /* Event for GUI startup */
  GUI_MNG_EV_LOAD_SENSOR_SCREEN,        /* Event for loading sensor screen */
  GUI_MNG_EV_SENSOR_DATA_UPDATE,        /* Event for updating sensor data */
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
typedef union 
{
  struct 
  {
    uint32_t reserved;
  } startup;

  struct 
  {
    uint32_t value;
  } number;

  struct 
  {
    uint16_t x;
    uint16_t y;
  } point;

  /* Payload for GUI_MNG_EV_SENSOR_DATA_UPDATE */
  struct 
  {
    int32_t  temperature; /* in 0.01 °C (e.g. 2150 = 21.50 °C) */
    uint32_t humidity;    /* in 1/1024 %RH (e.g. 59392 = 58.0 %RH) */
    uint32_t pressure;    /* in Pascals (e.g. 101325 = 1013.25 hPa) */
  } sensor_data;

} gui_mng_event_data_t;


/* Public Function Prototypes */
void gui_cfg_init( void );
void gui_cfg_mng_process( gui_mng_event_t event, const gui_mng_event_data_t *data );
void gui_cfg_refresh( void );

#endif /* MAIN_GUI_MNG_CFG_H_ */
