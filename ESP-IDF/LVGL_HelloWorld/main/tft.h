/*
 * tft.h
 *
 *  Created on: Dec 15, 2023
 *      Author: xpress_embedo
 */

#ifndef MAIN_TFT_H_
#define MAIN_TFT_H_

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "ili9341.h"
#include "xpt2046.h"

// Defines
// Display Resolution
#define TFT_HOR_RES_MAX               (240)
#define TFT_VER_RES_MAX               (320)

#define TFT_SPI_HOST                  (SPI3_HOST)
// Display Related Pins
#define TFT_SPI_MOSI                  (GPIO_NUM_23)
#define TFT_SPI_MISO                  (GPIO_NUM_19)
#define TFT_SPI_SCLK                  (GPIO_NUM_18)
#define TFT_SPI_CS                    (GPIO_NUM_5)
#define TFT_PIN_DC                    (GPIO_NUM_0)
#define TFT_PIN_RESET                 (-1)
// Touch Related SPI Pins (NOTE: Display and Touch SPI is considered as same,
// only chip select is different for obvious reasons
#define TOUCH_SPI_MOSI                (TFT_SPI_MOSI)
#define TOUCH_SPI_MISO                (TFT_SPI_MISO)
#define TOUCH_SPI_SCLK                (TFT_SPI_SCLK)
#define TOUCH_SPI_CS                  (GPIO_NUM_2)

#define TFT_CS_LOW()                 gpio_set_level(TFT_SPI_CS, 0)
#define TFT_CS_HIGH()                gpio_set_level(TFT_SPI_CS, 1)
#define TOUCH_CS_LOW()               gpio_set_level(TOUCH_SPI_CS, 0)
#define TOUCH_CS_HIGH()              gpio_set_level(TOUCH_SPI_CS, 1)

#define TFT_DC_LOW()                 gpio_set_level(TFT_PIN_DC, 0)
#define TFT_DC_HIGH()                gpio_set_level(TFT_PIN_DC, 1)

// Public Functions
void tft_init( void );
void tft_delay_ms(uint32_t delay);
void tft_send_cmd( uint8_t cmd, const uint8_t *data, int len );
void tft_send_data( const uint8_t *data, int len );
void touch_read_data( uint8_t cmd, uint8_t *data, int len );

#endif /* MAIN_TFT_H_ */
