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
  GUI_MNG_EV_WIFI_CONNECTING,     // Event for device connecting with WiFi
  GUI_MNG_EV_MQTT_CONNECTING,     // Event for device connecting with MQTT broker
  GUI_MNG_EV_MQTT_CONNECTED,      // Event for device connected with MQTT broker
  GUI_MNG_EV_TEMP_HUMID,          // Event for temperature and humidity update
  GUI_MNG_EV_SWITCH_LED,          // Event for Switch LED
  GUI_MNG_EV_RGB_LED,             // Event for RGB Led
  GUI_MNG_EV_MAX,
} gui_mng_event_t;



#endif /* MAIN_GUI_MNG_CFG_H_ */
