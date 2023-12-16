/*
 * ili9341.h
 *
 *  Created on: Dec 16, 2023
 *      Author: xpress_embedo
 */
#ifndef ILI9341_H
#define ILI9341_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>

#include "lvgl.h"

// Defines
#define ILI9341_DC              (DISP_PIN_DC)
#define ILI9341_USE_RST         (false)
#define ILI9341_RST             (DISP_PIN_RESET)
#define ILI9341_INVERT_COLORS   (false)

// Public Prototypes
void ili9341_init( void );
void ili9341_flush( lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_map );
void ili9341_sleep_in( void );
void ili9341_sleep_out( void );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*ILI9341_H*/
