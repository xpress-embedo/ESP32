/*
 * app_mqtt.h
 *
 *  Created on: Aug 8, 2025
 *      Author: xpress_embedo
 */

#ifndef MAIN_MQTT_APP_H_
#define MAIN_MQTT_APP_H_

// enumeration for led data of traffic lights
typedef enum _traffic_led_e
{
  TRAFFIC_LED_GREEN = 0,
  TRAFFIC_LED_YELLOW = 1,
  TRAFFIC_LED_RED = 2,
  TRAFFIC_LED_INVALID,
} traffic_led_e;

// Public Function Declaration
void mqtt_app_start( void );
void mqtt_app_mng( void );

#endif /* MAIN_MQTT_APP_H_ */
