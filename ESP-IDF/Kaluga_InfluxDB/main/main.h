/*
 * main.h
 *
 *  Created on: Jun 26, 2024
 *      Author: xpress_embedo
 */

#ifndef MAIN_MAIN_H_
#define MAIN_MAIN_H_

#include <unistd.h>

// macros
#define SENSOR_BUFF_SIZE                        (100u)
#define MAC_ADDR_SIZE                           (18u)

// Public Function Definition
uint8_t get_temperature( void );
uint8_t get_humidity( void );
void get_mac_address( char *mac_str );
long long get_time_ns( void );

#endif /* MAIN_MAIN_H_ */
