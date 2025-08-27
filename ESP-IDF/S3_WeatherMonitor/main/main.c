
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "main.h"
#include "gui_mng.h"
#include "dht11.h"

// Private Macros
#define MAIN_TASK_PERIOD                (5000)
#define DHT11_PIN              					(GPIO_NUM_17)

// Private Variables
static const char *TAG = "MAIN";
static sensor_data_t sensor_data = { .sensor_idx = 0 };

void app_main(void)
{
  dht11_sensor_t dht11_value;

  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if ( (ret == ESP_ERR_NVS_NO_FREE_PAGES) || (ret == ESP_ERR_NVS_NEW_VERSION_FOUND) )
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
  
	ESP_LOGI( TAG, "Starting Program");
	
  // initialize dht sensor library
  dht11_init(DHT11_PIN, true);
	
	gui_start();
	
  while(1)
  {
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
          sensor_data.humidity_current = temp;
          temp = (uint8_t)dht11_read().temperature;
          sensor_data.temperature[sensor_data.sensor_idx] = temp;
          sensor_data.temperature_current = temp;
          ESP_LOGI(TAG, "Temperature: %d", sensor_data.temperature_current);
          ESP_LOGI(TAG, "Humidity: %d", sensor_data.humidity_current);
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
        ESP_LOGE(TAG, "In-correct data received from DHT11 -> %u", temp);
      }
    }
    else
    {
      ESP_LOGE(TAG, "Unable to Read DHT11 Status");
    }
    // Wait before next measurement
    vTaskDelay(MAIN_TASK_PERIOD / portTICK_PERIOD_MS);
  }
}

// Public Function Definition
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
