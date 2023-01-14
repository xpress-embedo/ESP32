/*
 * Display Manager
 *
 *  Created on: 01-Nov-2022
 *      Author: xpress_embedo
 */

#ifndef INC_DISPLAY_MNG_H_
#define INC_DISPLAY_MNG_H_

#include "main.h"

typedef enum _Display_States_e
{
  DISP_STATE_INIT = 0,            // Configure Station Mode and Disconnect
  DISP_STATE_INIT_WAIT,           // Wait for sometime after initializing WiFi
  DISP_STATE_SCAN_SSID,           // Let the list of WiFi SSID's available
  DISP_STATE_SCAN_SSID_WAIT,      // Wait for sometime
  DISP_STATE_CONNECT_MENU,        // Show UI generated to connect to Router
  DISP_STATE_CONNECT_MENU_WAIT,   // Wait for user inputs to connect
  DISP_STATE_CONNECT_FAIL,        // If Connection to Router Fails
  DISP_STATE_CONNECTED,           // When Connected
  DISP_STATE_CONNECTED_WAIT,
  DISP_STATE_MAX
} Display_State_e;

void Display_Init( void );
void Display_Mng( void );
void Display_ChangeState( Display_State_e state );
Display_State_e Display_GetDispState( void );

#endif /* INC_DISPLAY_MNG_H_ */