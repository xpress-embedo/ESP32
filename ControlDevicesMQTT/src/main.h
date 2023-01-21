/*
 * Display Manager
 *
 *  Created on: 01-Nov-2022
 *      Author: xpress_embedo
 */

#ifndef INC_MAIN_H_
#define INC_MAIN_H_

#include <Arduino.h>

// Public Functions
char *Get_WiFiSSID_DD_List( void );
void WiFi_Init( void );
void WiFi_ScanSSID( void );

#endif /* INC_MAIN_H_ */