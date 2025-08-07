/*
 * gui_mng_cfg.h
 *
 *  Created on: Feb 24, 2024
 *      Author: xpress_embedo
 */

#ifndef MAIN_GUI_MNG_CFG_H_
#define MAIN_GUI_MNG_CFG_H_

typedef enum {
  GUI_MNG_EV_NONE = 0,
  GUI_MNG_EV_WIFI_CONNECTING,       // Event for device connecting with WiFi
  GUI_MNG_EV_MQTT_CONNECTING,       // Event for device connecting with MQTT broker
  GUI_MNG_EV_MQTT_CONNECTED,        // Event for device connected with MQTT broker
  GUI_MNG_EV_TRAFFIC_LED_1,         // Events for updating the traffic LEDs status of side-1
  GUI_MNG_EV_TRAFFIC_LED_2,         // Events for updating the traffic LEDs status of side-2
  GUI_MNG_EV_TRAFFIC_LED_3,         // Events for updating the traffic LEDs status of side-3
  GUI_MNG_EV_TRAFFIC_LED_4,         // Events for updating the traffic LEDs status of side-4
  GUI_MNG_EV_TRAFFIC_TIME_1,        // Events for updating the traffic LEDs time of side-1
  GUI_MNG_EV_TRAFFIC_TIME_2,        // Events for updating the traffic LEDs time of side-2
  GUI_MNG_EV_TRAFFIC_TIME_3,        // Events for updating the traffic LEDs time of side-3
  GUI_MNG_EV_TRAFFIC_TIME_4,        // Events for updating the traffic LEDs time of side-4
  GUI_MNG_EV_MAX,
} gui_mng_event_t;

void gui_cfg_init( void );
void gui_cfg_mng_process( gui_mng_event_t event, uint8_t *data );

#endif /* MAIN_GUI_MNG_CFG_H_ */
