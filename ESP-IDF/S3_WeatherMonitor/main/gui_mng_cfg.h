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
  GUI_MNG_EV_WIFI_CONNECTING,       	// Event for device connecting with WiFi
  GUI_MNG_EV_WIFI_CONNECTED,       		// Event for ESP32 is connected with Router
  GUI_MNG_EV_WIFI_DISCONNECTED,     	// Event for WiFi Disconnected
  GUI_MNG_EV_WIFI_INTERNET_CONNECTED, // Event for ESP32 has internet access as it can connect with SNTP server
  GUI_MNG_EV_LOAD_SENSOR_SCREEN,			// Event for loading sensor screen
  GUI_MNG_EV_TEMP_HUMID,							// Event for display temperature and humidity data
  GUI_MNG_EV_MAX,
} gui_mng_event_t;


// Public Function Prototypes
void gui_cfg_init( void );
void gui_cfg_mng_process( gui_mng_event_t event, uint8_t *data );
void gui_cfg_refresh( void );

#endif /* MAIN_GUI_MNG_CFG_H_ */
