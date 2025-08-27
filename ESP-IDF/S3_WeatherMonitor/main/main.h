/*
 * main.h
 *
 *  Created on: 26-Aug-2025
 *      Author: xpress_embedo
 */

#ifndef MAIN_MAIN_H_
#define MAIN_MAIN_H_

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "esp_log.h"
#include "freertos/idf_additions.h"

// Public Macros
#define SENSOR_BUFF_SIZE                        (100u)

// Data Structure
typedef struct _sensor_data_t
{
  uint8_t temperature_current;
  uint8_t humidity_current;
  uint8_t temperature[SENSOR_BUFF_SIZE];
  uint8_t humidity[SENSOR_BUFF_SIZE];
  size_t  sensor_idx;
} sensor_data_t;

// Public Function Definition
sensor_data_t * get_temperature_humidity( void );

#endif /* MAIN_MAIN_H_ */
