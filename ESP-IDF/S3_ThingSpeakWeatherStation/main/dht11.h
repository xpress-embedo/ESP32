/*
 * dht11.h
 *
 *  Created on: 22-Aug-2023
 *      Author: xpress_embedo
 */

#ifndef MAIN_DHT11_H_
#define MAIN_DHT11_H_

#include "driver/gpio.h"

/* Project Specific Enumerations */
typedef enum _dht11_status_e
{
  DHT11_CHECKSUM_ERROR = -2,
  DHT11_TIMEOUT_ERROR = -1,
  DHT11_OK = 0,
} dht11_status_e;

/* Project Specific Data Structure */
typedef struct _dht11_reading_t
{
  int status;
  int temperature;
  int humidity;
} dht11_reading_t;

/* Public Function Prototypes */
void dht11_init( gpio_num_t gpio_num, uint8_t start_delay );
dht11_reading_t dht11_read( void );

#endif /* MAIN_DHT11_H_ */
