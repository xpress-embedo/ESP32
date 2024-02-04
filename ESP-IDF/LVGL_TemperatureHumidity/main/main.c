#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "dht11.h"
#include "gui_mng.h"

// macros
#define DHT11_PIN                     (GPIO_NUM_12)
#define MAIN_TASK_PERIOD              (5000)

// Private Variables
static const char *TAG = "APP";
static uint8_t temperature = 0;
static uint8_t humidity = 0;

// Private Function Declarations
// todo

void app_main(void)
{
  // Disable default gpio logging messages
  esp_log_level_set("gpio", ESP_LOG_NONE);

  // initialize dht sensor library
  dht11_init(DHT11_PIN);

  // start the gui task, this handles all the display related stuff
  gui_start();

  while (true)
  {
    vTaskDelay(MAIN_TASK_PERIOD / portTICK_PERIOD_MS);
    // Get DHT11 Temperature and Humidity Values
    if( dht11_read().status == DHT11_OK )
    {
      uint8_t temp = (uint8_t)dht11_read().humidity;
      if( temp < 100 )
      {
        // humidity can't be greater than 100%, that means invalid data
        humidity = temp;
        temperature = (uint8_t)dht11_read().temperature;
        ESP_LOGI(TAG, "Temperature: %d", temperature);
        ESP_LOGI(TAG, "Humidity: %d", humidity);
        // trigger event to display temperature and humidity
        gui_set_event(GUI_MNG_EV_TEMP_HUMID);
      }
      else
      {
        ESP_LOGI(TAG, "In-correct data received from DHT11");
      }
    }
    else
    {
      ESP_LOGI(TAG, "Unable to Read DHT11 Status");
    }
  }
}

// Private Function Definitions

uint8_t get_temerature( void )
{
  return temperature;
}

uint8_t get_humidity( void )
{
  return humidity;
}


