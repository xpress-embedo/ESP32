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
#define GUI_MNG_REFRESH_TIME            (20)    // in milliseconds

// Enumeration
typedef enum {
  GUI_MNG_EV_NONE = 0,
  GUI_MNG_EV_WIFI_CONNECTING,       // Event for device connecting with WiFi
  GUI_MNG_EV_MQTT_CONNECTING,       // Event for device connecting with MQTT broker
  GUI_MNG_EV_MQTT_CONNECTED,        // Event for device connected with MQTT broker
  GUI_MNG_EV_WIFI_DISCONNECTED,     // Event for WiFi Disconnected
  GUI_MNG_EV_LOAD_SENSOR_SCREEN,		// 
  GUI_MNG_EV_MAX,
} gui_mng_event_t;


// Public Function Prototypes
void gui_cfg_init( void );
void gui_cfg_mng_process( gui_mng_event_t event, uint8_t *data );
void gui_cfg_refresh( void );

#endif /* MAIN_GUI_MNG_CFG_H_ */
