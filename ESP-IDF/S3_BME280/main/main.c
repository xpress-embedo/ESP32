#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "i2c_bus.h"
#include "bme280.h"

#include "gui_mng.h"

/* Private Macros */
#define MAIN_TASK_PERIOD                                    (1000u)     // Main task period in milliseconds
// External I2C bus (BME280) uses a different port than the touch controller's I2C_NUM_0.
#define I2C_MASTER_NUM                                       I2C_NUM_1

/* Private Variables */
static const char *TAG = "MAIN";
static i2c_bus_handle_t i2c_bus = NULL;
static bme280_handle_t bme280 = NULL;

void app_main(void)
{
  ESP_LOGI( TAG, "Program Started" );

  /* I2C Bus Configuration */
  i2c_config_t i2c_cfg = 
  {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = 17,  // GPIO number for SDA
    .scl_io_num = 18,  // GPIO number for SCL
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = 100000, // 100kHz
  };

  i2c_bus = i2c_bus_create( I2C_MASTER_NUM, &i2c_cfg );

  /* Step2: Init bme280 */
  bme280 = bme280_create( i2c_bus, BME280_I2C_ADDRESS_DEFAULT );
  bme280_default_init( bme280 );

  if ( !gui_start() )
  {
    ESP_LOGE( TAG, "GUI startup failed" );
    return;
  }

  while(1)
  {
    //Step3: Read temperature, humidity and pressure
    float temperature = 0.0, humidity = 0.0, pressure = 0.0;
    bme280_read_temperature( bme280, &temperature );
    bme280_read_humidity( bme280, &humidity );
    bme280_read_pressure( bme280, &pressure );
    ESP_LOGI( TAG, "Temperature: %.2f C, Humidity: %.2f %%RH, Pressure: %.2f hPa", temperature, humidity, pressure );
    vTaskDelay( pdMS_TO_TICKS( 1000 ) );
  }
}
