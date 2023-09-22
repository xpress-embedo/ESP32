/*
 * wifi_reset_button.h
 *
 *  Created on: 22-Sep-2023
 *      Author: xpress_embedo
 */

#ifndef MAIN_WIFI_RESET_BUTTON_H_
#define MAIN_WIFI_RESET_BUTTON_H_

// Macros
// Default Interrupt Flag
#define ESP_INTR_FLAG_DEFAULT         (0)
// WiFi Reset Button (on Kaluga Development Kit, I am using GPIO6, which is present
// on the Audio board, strange thing is that they are using GPIO6 for all six
// buttons as for the application these button presses are sensed using ADC, but
// for my application I am using it as normal GPIO, and K6 is specifically used for this.
#define WIFI_RESET_BUTTON             (GPIO_NUM_6)   // GPIO6

// Public Function Declaration
void wifi_reset_button_config( void );


#endif /* MAIN_WIFI_RESET_BUTTON_H_ */
