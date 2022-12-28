/*
 * rgb_led.h
 *
 *  Created on: 28-Dec-2022
 *      Author: xpress_embedo
 */

#ifndef MAIN_RGB_LED_H_
#define MAIN_RGB_LED_H_


// RGB LED GPIO
#define RGB_LED_RED_GPIO            (21u)
#define RGB_LED_GREEN_GPIO          (22u)
#define RGB_LED_BLUE_GPIO           (23u)

// RGB LED Color Mix Channels
#define RGB_LED_CHANNEL_NUM         (3u)

// RGB LED Configuration Structure
typedef struct
{
  int channel;
  int gpio;
  int mode;
  int timer_idx;
} Led_Info_s;

// Public Function Prototypes
void RGB_PWM_Init( void );
void RGB_WiFi_Started( void );
void RGB_HTTP_ServerStarted( void );
void RGB_WiFi_Connected( void );
/*
 * Color to Indicate WiFi application has started
 */

#endif /* MAIN_RGB_LED_H_ */
