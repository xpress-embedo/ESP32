/*
 * display_mng.h
 *
 *  Created on: Jan 26, 2024
 *      Author: xpress_embedo
 */

#ifndef MAIN_DISPLAY_MNG_H_
#define MAIN_DISPLAY_MNG_H_

#include "tft.h"

// Public Function Prototypes
void display_init( void );
uint8_t display_update_lock( void );
void display_update_unlock( void );


#endif /* MAIN_DISPLAY_MNG_H_ */
