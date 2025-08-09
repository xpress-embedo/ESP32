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

/*
 * Message IDs for the MQTT Application Task
 */
typedef enum _mqtt_app_msg_e
{
  MQTT_APP_MSG_START_CONNECTION = 0,
  MQTT_APP_MSG_STOP_CONNECTION,
  MQTT_APP_MSG_PUBLISH_XYZ,         // todo: for future if needed
} mqtt_app_msg_e;

/*
 * Structure for MQTT Message Queue
 */
typedef struct _mqtt_app_q_msg
{
  mqtt_app_msg_e msg_id;
} mqtt_app_q_msg_t;

// Public Function Declaration
void mqtt_app_start( void );
BaseType_t mqtt_app_send_msg( mqtt_app_msg_e msg_id );

#endif /* MAIN_MQTT_APP_H_ */
