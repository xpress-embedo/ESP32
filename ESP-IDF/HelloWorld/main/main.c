#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

// Macros
#define BLINK_GPIO              (45)
#define BLINK_GPIO_PERIOD       (500)

// Private Variables
static const char *TAG = "APP";
static uint8_t s_led_state = 0;
static led_strip_handle_t led_strip;

// Private Function Prototypes
static void blink_led(void);
static void configure_led(void);

// Main Program Starts from here
void app_main(void)
{
  /* Configure the peripheral according to the LED type */
  configure_led();

  while (1)
  {
    ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
    blink_led();
    /* Toggle the LED state */
    s_led_state = !s_led_state;
    vTaskDelay(BLINK_GPIO_PERIOD / portTICK_PERIOD_MS);
  }
}

// Private Function Definition
static void blink_led(void)
{
  /* If the addressable LED is enabled */
  if (s_led_state)
  {
    /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
    led_strip_set_pixel(led_strip, 0, 255, 0, 0);
    /* Refresh the strip to send data */
    led_strip_refresh(led_strip);
  }
  else
  {
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
  }
}

static void configure_led(void)
{
  ESP_LOGI(TAG, "Blink LED");
  /* LED strip initialization with the GPIO and pixels number*/
  led_strip_config_t strip_config =
  {
    .strip_gpio_num = BLINK_GPIO,
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
