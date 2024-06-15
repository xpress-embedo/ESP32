#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "dht11.h"

// macros
#define DHT11_PIN                           (GPIO_NUM_17)
#define MAIN_TASK_PERIOD                    (6000)

// Private Variables
static const char *TAG = "APP";

void app_main(void)
{
  // Disable default gpio logging messages
  esp_log_level_set("gpio", ESP_LOG_NONE);

  // initialize dht sensor library
  dht11_init(DHT11_PIN, true);

  while(1)
  {
    // Get DHT11 Temperature and Humidity Values
    if( dht11_read().status == DHT11_OK )
    {
      uint8_t humidity = (uint8_t)dht11_read().humidity;
      uint8_t temperature = (uint8_t)dht11_read().temperature;
      ESP_LOGI(TAG, "Temperature: %d", temperature);
      ESP_LOGI(TAG, "Humidity: %d", humidity);
    }
    else
    {
      ESP_LOGE(TAG, "Unable to Read DHT11 Status");
    }
    // Wait before next measurement
    vTaskDelay(MAIN_TASK_PERIOD / portTICK_PERIOD_MS);
  }
}
