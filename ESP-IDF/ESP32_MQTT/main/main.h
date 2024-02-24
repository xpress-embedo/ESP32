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

#endif /* MAIN_MAIN_H_ */