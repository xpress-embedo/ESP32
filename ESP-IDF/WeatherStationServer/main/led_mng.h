/*
 * led_mng.h
 *
 *  Created on: 14-Jul-2023
 *      Author: xpress_embedo
 */

#ifndef LED_MNG_H_
#define LED_MNG_H_

#include "prj_refs.h"

// Macros
// Some Helper Macros
#define LED_RED_ON()                        SAFE_CALL(led_set_color(255, 0, 0))
#define LED_GREEN_ON()                      SAFE_CALL(led_set_color(0, 255, 0))
#define LED_BLUE_ON()                       SAFE_CALL(led_set_color(0, 0, 255))

// Public Definitions
void led_init(void);
void led_set_color(uint8_t red, uint8_t green, uint8_t blue);

#endif /* LED_MNG_H_ */
