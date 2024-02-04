#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"
#include "display_mng.h"
#include "dht11.h"
#include "ui.h"

typedef enum {
  LANGUAGE_ENGLISH = 0,
  LANGUAGE_GERMAN,
  LANGUAGE_HINDI,
  LANGUAGE_MAX
} language_t;

// macros
#define DHT11_PIN                     (GPIO_NUM_12)
#define MAIN_TASK_PERIOD              (5000)

// Private Variables
static const char *TAG = "APP";
static int temperature = 0;
static int humidity = 0;

// Private Function Declarations
static void update_temperature_humidity( void );

void app_main(void)
{
  // Disable default gpio logging messages
  esp_log_level_set("gpio", ESP_LOG_NONE);

  // initialize dht sensor library
  dht11_init(DHT11_PIN);

  // initialize display related stuff, also lvgl
  display_init();

  // main user interface
  ui_init();

  while (true)
  {
    vTaskDelay(MAIN_TASK_PERIOD / portTICK_PERIOD_MS);
    // Get DHT11 Temperature and Humidity Values
    if( dht11_read().status == DHT11_OK )
    {
      temperature = (uint8_t)dht11_read().temperature;
      humidity = (uint8_t)dht11_read().humidity;
      ESP_LOGI(TAG, "Temperature: %d", temperature);
      ESP_LOGI(TAG, "Humidity: %d", humidity);
      update_temperature_humidity();
    }
    else
    {
      ESP_LOGI(TAG, "Unable to Read DHT11 Status");
    }
  }
}

// Private Function Definitions
/*
 * @brief Update the temperature and humidity values of the display
 */
static void update_temperature_humidity( void )
{
  if( true == display_update_lock() )
  {
    lv_label_set_text_fmt(ui_lblTemperatureValue, "%d °C", temperature);
    lv_label_set_text_fmt(ui_lblHumidityValue, "%d %%", humidity);
    // ESP_LOGI(TAG, "Variable Temperature = %d, Humidity = %d Updated on Display", temperature, humidity);
    // ESP_LOGI(TAG, "Label => Temperature = %s, Humidity = %s", lv_label_get_text(ui_lblTemperatureValue), lv_label_get_text(ui_lblHumidityValue));
    display_update_unlock();
  }
}


