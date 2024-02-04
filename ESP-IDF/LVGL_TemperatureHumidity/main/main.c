#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "main.h"
#include "dht11.h"
#include "gui_mng.h"

// macros
#define DHT11_PIN                     (GPIO_NUM_12)
#define MAIN_TASK_PERIOD              (5000)

// Private Variables
static const char *TAG = "APP";
static sensor_data_t sensor_data = { .sensor_idx = 0 };

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
      // humidity can't be greater than 100%, that means invalid data
      if( temp < 100 )
      {
        if( sensor_data.sensor_idx < SENSOR_BUFF_SIZE )
        {
          sensor_data.humidity[sensor_data.sensor_idx] = temp;
          temp = (uint8_t)dht11_read().temperature;
          sensor_data.temperature[sensor_data.sensor_idx] = temp;
          ESP_LOGI(TAG, "Temperature: %d", sensor_data.temperature[sensor_data.sensor_idx]);
          ESP_LOGI(TAG, "Humidity: %d", sensor_data.humidity[sensor_data.sensor_idx]);
          // trigger event to display temperature and humidity
          gui_set_event(GUI_MNG_EV_TEMP_HUMID);
          sensor_data.sensor_idx++;
          // reset the index
          if( sensor_data.sensor_idx >= SENSOR_BUFF_SIZE )
          {
            sensor_data.sensor_idx = 0;
          }
        }
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

// Public Function Definitions

/**
 * @brief Get the Pointer to the Sensor Data Structure to get the temperature 
 *        and Humidity values
 * @param  None
 * @return sensor_data data structure pointer
 */
sensor_data_t * get_temperature_humidity( void )
{
  return &sensor_data;
}

// Private Function Definitions




