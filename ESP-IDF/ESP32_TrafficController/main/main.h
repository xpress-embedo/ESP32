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

#include "driver/gpio.h"

// Public Macros
#define TRAFFIC_LIGHT_SIDES                   (4u)

// Structure Declaration
// structure for traffic light time as per version 2
typedef struct _traffic_light
{
  char color;
  uint8_t green_time;
  uint8_t yellow_time;
  uint8_t red_time;
} traffic_light_t;

// Global Variables
extern const uint8_t TAB_GREEN_LIGHT[TRAFFIC_LIGHT_SIDES];
extern const uint8_t TAB_YELLOW_LIGHT[TRAFFIC_LIGHT_SIDES];
extern const uint8_t TAB_RED_LIGHT[TRAFFIC_LIGHT_SIDES];

// Public Function Declaration
void traffic_lights_on( gpio_num_t gpio_num );
void traffic_lights_off( gpio_num_t gpio_num );


#endif /* MAIN_MAIN_H_ */
