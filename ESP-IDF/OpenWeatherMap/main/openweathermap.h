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
