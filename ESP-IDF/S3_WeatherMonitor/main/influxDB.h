/*
 * influxDB.h
 *
 *  Created on: Jun 21, 2024
 *      Author: xpress_embedo
 */

#ifndef MAIN_INFLUXDB_H_
#define MAIN_INFLUXDB_H_

#include <unistd.h>

#include "freertos/FreeRTOS.h"

typedef enum {
  INFLUXDB_EV_NONE = 0,
  INFLUXDB_EV_TEMP_HUMID = 1,   // Event for temperature and humidity update
  INFLUXDB_EV_MAX,
} influxdb_event_t;

typedef struct _influxdb_q_msg_t {
  influxdb_event_t  event_id;
  uint8_t           *data;
} influxdb_q_msg_t;

// Public Function Prototypes
void influxdb_start( void );
BaseType_t influxdb_send_event( influxdb_event_t event, uint8_t *pData );

#endif /* MAIN_INFLUXDB_H_ */
