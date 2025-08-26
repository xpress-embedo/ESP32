
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "main.h"
#include "gui_mng.h"
#include "dht11.h"

// Private Macros
#define MAIN_TASK_PERIOD                (5000)
#define DHT11_GPIO_NUM              		(GPIO_NUM_17)

// Private Variables
static const char *TAG = "MAIN";
static uint8_t humidity = 0u;
static uint8_t temperature = 0u;

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
	
	// initialize the dht11 module
  dht11_init(DHT11_GPIO_NUM);
	
	gui_start();
	
	while( 1 )
	{
		// read the data from dht11 sensor
    dht11_value = dht11_read();
    // If reading is valid then only print
    if( dht11_value.status == DHT11_OK )
    {
      temperature = (uint8_t)dht11_value.temperature;
      humidity = (uint8_t)dht11_value.humidity;
      ESP_LOGI(TAG, "Temperature:%d, Humidity:%d", temperature, humidity);
    }
    vTaskDelay(MAIN_TASK_PERIOD / portTICK_PERIOD_MS);
	}
}
