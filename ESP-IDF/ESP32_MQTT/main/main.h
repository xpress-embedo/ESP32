/*
 * main.h
 *
 *  Created on: Feb 11, 2024
 *      Author: xpress_embedo
 */

#ifndef MAIN_MAIN_H_
#define MAIN_MAIN_H_

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

// Sensor data structure
typedef struct _sensor_data_t
{
  uint8_t temperature;
  uint8_t humidity;
} sensor_data_t;

// Public Function Declaration
void app_publish_switch_led( bool status );
void app_publish_sensor_data( void );
void app_publish_slider_data( void );

#endif /* MAIN_MAIN_H_ */
