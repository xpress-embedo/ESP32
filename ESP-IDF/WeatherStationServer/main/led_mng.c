/*
 * led_mng.c
 *
 *  Created on: 14-Jul-2023
 *      Author: xpress_embedo
 */

#include "led_mng.h"
#include "led_strip.h"
#include "esp_log.h"

// Macros
#define LED_GPIO                            (45)

// Private Variables
static led_strip_handle_t led_strip;

// Public Function Definition
void led_init(void)
{
  /* LED strip initialization with the GPIO and pixels number*/
  led_strip_config_t strip_config =
  {
    .strip_gpio_num = LED_GPIO,
    .max_leds = 1, // at least one LED on board
  };
  led_strip_rmt_config_t rmt_config =
  {
    .resolution_hz = 10 * 1000 * 1000, // 10MHz
  };

  ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
  /* Set all LED off to clear all pixels */
  led_strip_clear(led_strip);
}

void led_set_color(uint8_t red, uint8_t green, uint8_t blue)
{
  // led_strip_clear(led_strip);
  led_strip_set_pixel(led_strip, 0, red, green, blue);
  led_strip_refresh(led_strip);
}
