/*
 * main.c
 *
 *  Created on: Aug 30, 2026
 *      Author: xpress_embedo
 */

#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_bus.h"
#include "bme280.h"

#include "gui_mng.h"

/* Private Macros */
#define MAIN_TASK_PERIOD_MS                  (2000u)     /*!< Sensor reading & GUI update period in ms */

/**
 * @brief I2C Port and Pin Assignment for BME280 Sensor
 * 
 * Note: GT911 Capacitive Touch Controller is independently configured on I2C_NUM_0 
 * using IO19 (SDA) and IO20 (SCL). BME280 uses a dedicated I2C_NUM_1 controller on
 * IO12 (SDA) and IO13 (SCL) to avoid bus contention.
 */
#define BME280_I2C_PORT                      I2C_NUM_1
#define BME280_I2C_SDA_IO                    (12)
#define BME280_I2C_SCL_IO                    (13)
#define BME280_I2C_CLK_SPEED_HZ              (100000)    /*!< Standard 100 kHz I2C clock speed */

/* Private Variables */
static const char *TAG = "MAIN";
static i2c_bus_handle_t s_i2c_bus = NULL;
static bme280_handle_t  s_bme280  = NULL;

/* Private Function Prototypes */
static esp_err_t bme280_init( void );
static void bme280_get_and_update_gui( void );

/* Public Function Definition */

/**
 * @brief Application Main Entry Point
 *
 * Initializes the BME280 environment sensor on dedicated I2C1, starts the
 * LVGL GUI manager for the 4.3" 800x480 RGB display, and continuously samples
 * and updates weather telemetry.
 */
void app_main( void )
{
  ESP_LOGI( TAG, "========================================================" );
  ESP_LOGI( TAG, "  ESP32-S3 BME280 Weather Station (4.3\" 800x480 LCD)" );
  ESP_LOGI( TAG, "========================================================" );

  /* Initialize BME280 Sensor */
  if ( ESP_OK != bme280_init() )
  {
    ESP_LOGE( TAG, "Sensor initialization failed! Halting sensor updates." );
    return;
  }

  /* Initialize and Start GUI Manager */
  if ( !gui_start() )
  {
    ESP_LOGE( TAG, "GUI manager startup failed!" );
    return;
  }
  ESP_LOGI( TAG, "Weather station running. Sampling every %u ms...", MAIN_TASK_PERIOD_MS );

  /* Main Measurement & Telemetry Loop */
  while ( 1 )
  {
    bme280_get_and_update_gui();
    vTaskDelay( pdMS_TO_TICKS( MAIN_TASK_PERIOD_MS ) );
  }
}

/* Private Function Definitions */

/**
 * @brief Initialize the dedicated I2C bus and BME280 environmental sensor.
 * 
 * @return esp_err_t ESP_OK on success, or appropriate error code.
 */
static esp_err_t bme280_init( void )
{
  /* Configure I2C bus parameters */
  const i2c_config_t i2c_cfg = 
  {
    .mode             = I2C_MODE_MASTER,
    .sda_io_num       = BME280_I2C_SDA_IO,
    .scl_io_num       = BME280_I2C_SCL_IO,
    .sda_pullup_en    = GPIO_PULLUP_ENABLE,
    .scl_pullup_en    = GPIO_PULLUP_ENABLE,
    .master.clk_speed = BME280_I2C_CLK_SPEED_HZ,
  };

  s_i2c_bus = i2c_bus_create( BME280_I2C_PORT, &i2c_cfg );
  if ( NULL == s_i2c_bus )
  {
    ESP_LOGE( TAG, "Failed to create I2C bus on port %d", BME280_I2C_PORT );
    return ESP_FAIL;
  }

  /* Create and configure BME280 device at default address 0x76 */
  s_bme280 = bme280_create( s_i2c_bus, BME280_I2C_ADDRESS_DEFAULT );
  if ( NULL == s_bme280 )
  {
    ESP_LOGE( TAG, "Failed to create BME280 driver handle" );
    return ESP_FAIL;
  }

  /* Soft-reset, calibration loading, and default sampling configuration */
  esp_err_t ret = bme280_default_init( s_bme280 );
  if ( ESP_OK != ret )
  {
    ESP_LOGE( TAG, "BME280 default configuration failed (err: 0x%X)", ret );
    return ret;
  }

  ESP_LOGI( TAG, "BME280 sensor initialized successfully on I2C%d (SDA=IO%d, SCL=IO%d)",
            BME280_I2C_PORT, BME280_I2C_SDA_IO, BME280_I2C_SCL_IO );
  return ESP_OK;
}

/**
 * @brief Read telemetry from BME280 and dispatch update event to the GUI.
 */
static void bme280_get_and_update_gui( void )
{
  float temperature = 0.0f;
  float humidity    = 0.0f;
  float pressure    = 0.0f;

  esp_err_t t_ret = bme280_read_temperature( s_bme280, &temperature );
  esp_err_t h_ret = bme280_read_humidity( s_bme280, &humidity );
  esp_err_t p_ret = bme280_read_pressure( s_bme280, &pressure );

  if ( ( ESP_OK == t_ret ) && ( ESP_OK == h_ret ) && ( ESP_OK == p_ret ) )
  {
    ESP_LOGI( TAG, "Temp: %.2f °C | Humidity: %.2f %%RH | Pressure: %.2f hPa",
              temperature, humidity, pressure );

    /* Format telemetry for GUI manager event queue */
    gui_mng_event_data_t event_data;
    event_data.sensor_data.temperature = (int32_t)( temperature * 100.0f );
    event_data.sensor_data.humidity    = (uint32_t)( humidity * 1024.0f );
    event_data.sensor_data.pressure    = (uint32_t)( pressure * 100.0f );

    /* Send payload to GUI manager task */
    gui_send_event( GUI_MNG_EV_SENSOR_DATA_UPDATE, &event_data );
  }
  else
  {
    ESP_LOGW( TAG, "Sensor read error (T: 0x%X, H: 0x%X, P: 0x%X)", t_ret, h_ret, p_ret );
  }
}


