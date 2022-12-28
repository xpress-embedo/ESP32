#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <dht.h>

#if defined(CONFIG_EXAMPLE_TYPE_DHT11)
#define SENSOR_TYPE         (DHT_TYPE_DHT11)
#endif
#if defined(CONFIG_EXAMPLE_TYPE_AM2301)
#define SENSOR_TYPE         (DHT_TYPE_AM2301)
#endif
#if defined(CONFIG_EXAMPLE_TYPE_SI7021)
#define SENSOR_TYPE         (DHT_TYPE_SI7021)
#endif

#define SENSOR_PIN          (CONFIG_EXAMPLE_DATA_GPIO)

static void DHT_Measure( void *pvParameters );

void app_main(void)
{
  // Create a task for reading DHT sensor data
  xTaskCreate( DHT_Measure, "DHT Measure", configMINIMAL_STACK_SIZE*3u, NULL, 5, NULL);
  while (true)
  {
    printf("Hello from app_main!\n");
    vTaskDelay( pdMS_TO_TICKS(1000) );
  }
}

static void DHT_Measure( void *pvParameters )
{
  float temperature, humidity;

  #ifdef CONFIG_EXAMPLE_INTERNAL_PULLUP
  // enable the pull-up on the data line if needed
  gpio_set_pull_mode( SENSOR_PIN, GPIO_PULLUP_ONLY );
  #endif

  while(1)
  {
    if( dht_read_float_data( SENSOR_TYPE, SENSOR_PIN, &humidity, &temperature) == ESP_OK )
    {
      printf("Humidity: %.1f%% Temp: %.1fC\n", humidity, temperature);
    }
    else
    {
      printf("Could not read data from sensor\n");
    }
    // don't read too fast from the sensor
    vTaskDelay( pdMS_TO_TICKS(2000) );
  }
}
