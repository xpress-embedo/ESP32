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
#define OPENWEATHERMAP_TASK_STACK_SIZE        (1024u*10)
#define OPENWEATHERMAP_TASK_PRIORITY          (5u)

// Public Function Prototypes
void openweathermap_init(void);
void openweathermap_mng(void);
void openweathermap_task(void *pvParameters);
int openweathermap_get_temperature(uint8_t city_idx);
int openweathermap_get_pressure(uint8_t city_idx);
int openweathermap_get_humidity(uint8_t city_idx);
uint8_t openweathermap_get_numofcity(void);

/*
To Create Task use the following code
xTaskCreate( &openweathermap_task,            \
             OPENWEATHERMAP_TASK_NAME,        \
             OPENWEATHERMAP_TASK_STACK_SIZE,  \
             NULL,                            \
             OPENWEATHERMAP_TASK_PRIORITY,    \
             NULL);
*/

#endif /* MAIN_OPENWEATHERMAP_H_ */
