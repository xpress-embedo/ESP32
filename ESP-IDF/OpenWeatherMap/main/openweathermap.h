/*
 * openweathermap.h
 *
 *  Created on: 07-Jul-2023
 *      Author: xpress_embedo
 */

#ifndef MAIN_OPENWEATHERMAP_H_
#define MAIN_OPENWEATHERMAP_H_

#include "main.h"

// Macros
#define OPENWEATHERMAP_TASK_NAME              "OpenWeatherMap"
#define OPENWEATHERMAP_TASK_STACK_SIZE        (2048u)
#define OPENWEATHERMAP_TASK_PRIORITY          (5u)

// Public Function Prototypes
void openweathermap_task(void *pvParameters);


#endif /* MAIN_OPENWEATHERMAP_H_ */
