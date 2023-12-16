/*
 * display_mng.h
 *
 *  Created on: Dec 15, 2023
 *      Author: xpress_embedo
 */

#ifndef MAIN_DISPLAY_MNG_H_
#define MAIN_DISPLAY_MNG_H_

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

// Defines
#define DISP_SPI_HOST                 (SPI3_HOST)
// Display Related Pins
#define DISP_SPI_MOSI                 (GPIO_NUM_23)
#define DISP_SPI_MISO                 (GPIO_NUM_19)
#define DISP_SPI_SCLK                 (GPIO_NUM_18)
#define DISP_SPI_CS                   (GPIO_NUM_5)
#define DISP_PIN_DC                   (GPIO_NUM_0)
#define DISP_PIN_RESET                (-1)
// Touch Related SPI Pins (NOTE: Display and Touch SPI is considered as same,
// only chip select is different for obvious reasons
#define TOUCH_SPI_MOSI                (DISP_SPI_MOSI)
#define TOUCH_SPI_MISO                (DISP_SPI_MISO)
#define TOUCH_SPI_SCLK                (DISP_SPI_SCLK)
#define TOUCH_SPI_CS

// Public Functions
void display_init( void );
void display_mng( void );
void display_send_cmd( uint8_t cmd );
void display_send_data( const uint8_t *data, int len );

#endif /* MAIN_DISPLAY_MNG_H_ */
