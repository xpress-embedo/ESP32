#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_wifi.h"

#include "main.h"
#include "dht11.h"
#include "thingspeak.h"
#include "gui_mng.h"

// macros
#define DHT11_PIN                           (GPIO_NUM_17)
#define MAIN_TASK_PERIOD                    (60000)
#define APP_WIFI_SSID                       "WiFi SSID"
#define APP_WIFI_PSWD                       "WiFi Password"
#define WIFI_MAX_RETRY                      (5)
#define WIFI_CONNECT_DELAY                  (500)     // Initial delay in milliseconds
#define WIFI_MAX_DELAY                      (60000)   // Maximum delay in milliseconds
// The following are the bits/flags for event group
#define WIFI_CONNECTED_BIT                  BIT0      // connected to the access point with an IP
#define WIFI_FAIL_BIT                       BIT1      // failed to connect after the max. amount of retries

// Private Variables
static const char *TAG = "APP";
/* Sensor Related Variables */
static sensor_data_t sensor_data = { .sensor_idx = 0 };
/* WiFi Connection Related Variables */
static EventGroupHandle_t wifi_event_group;           // FreeRTOS event group to signal when we are connected
static uint8_t wifi_connect_retry = 0;
static bool wifi_connect_status = false;

// Private Function Declarations
static void app_connect_wifi( void );
static void wifi_event_handler( void *arg, esp_event_base_t event_base, int32_t event_id, void * event_data );

void app_main(void)
{
  // Disable default gpio logging messages
  esp_log_level_set("gpio", ESP_LOG_NONE);
  // disable default wifi logging messages
  esp_log_level_set("wifi", ESP_LOG_NONE);

  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // connect with WiFi (it will take some time)
  app_connect_wifi();
  if( wifi_connect_status )
  {
    thingspeak_start();
  }

  // initialize dht sensor library
  dht11_init(DHT11_PIN, true);

  // start the gui task, this handles all the display related stuff
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
          // trigger event to display temperature and humidity
          gui_send_event(GUI_MNG_EV_TEMP_HUMID, (uint8_t*)(&sensor_data) );
          // if wifi is connected, trigger event to send data to ThingSpeak
          if( wifi_connect_status )
          {
            thingspeak_send_event(THING_SPEAK_EV_TEMP_HUMID, NULL);
          }
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
/**
 * @brief Connect with the WiFi Router
 * @note  in future this function can be moved to a commom place.
 * @param  none
 */
static void app_connect_wifi( void )
{
  wifi_event_group = xEventGroupCreate();

  ESP_ERROR_CHECK( esp_netif_init() );

  ESP_ERROR_CHECK( esp_event_loop_create_default() );
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;
  ESP_ERROR_CHECK( esp_event_handler_instance_register( WIFI_EVENT,           \
                                                        ESP_EVENT_ANY_ID,     \
                                                        &wifi_event_handler,  \
                                                        NULL,                 \
                                                        &instance_any_id) );
  ESP_ERROR_CHECK( esp_event_handler_instance_register( IP_EVENT,             \
                                                        IP_EVENT_STA_GOT_IP,  \
                                                        &wifi_event_handler,  \
                                                        NULL,                 \
                                                        &instance_got_ip) );

  wifi_config_t wifi_config =
  {
    .sta =
    {
      .ssid = APP_WIFI_SSID,
      .password = APP_WIFI_PSWD,
      .threshold.authmode = WIFI_AUTH_WPA2_PSK,
    },
  };

  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
  ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
  ESP_ERROR_CHECK( esp_wifi_start() );

  ESP_LOGI(TAG, "WiFi Initialized in Station Mode Finished.");

  /*
   * Wait until either the connection is established (WIFI_CONNECTED_BIT) or
   * connection failed for the maximum number of re-tries (WIFI_FAIL_BIT).
   * The bits are set by event_handler() (see above) */
  EventBits_t bits = xEventGroupWaitBits( wifi_event_group,                   \
                                          WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, \
                                          pdFALSE,                            \
                                          pdFALSE,                            \
                                          portMAX_DELAY);

  /* xEventGroupWaitBits() returns the bits before the call returned, hence we
   * can test which event actually happened. */
  if( bits & WIFI_CONNECTED_BIT )
  {
    ESP_LOGI(TAG, "Connected to Access Point %s", APP_WIFI_SSID );
  }
  else if( bits & WIFI_FAIL_BIT )
  {
    ESP_LOGE(TAG, "Failed to Connect to Access Point %s", APP_WIFI_SSID );
  }
  else
  {
    ESP_LOGE(TAG, "Unexpected Event" );
  }
  vEventGroupDelete(wifi_event_group);
}

/**
 * @brief WiFi Event Handler Function
 * @param arg 
 * @param event_base Event Base whether WIFI Event or IP Event
 * @param event_id   Event ID 
 * @param event_data Data with Event
 */
static void wifi_event_handler( void *arg, esp_event_base_t event_base, int32_t event_id, void * event_data )
{
  if( WIFI_EVENT == event_base )
  {
    if( WIFI_EVENT_STA_START == event_id )
    {
      esp_wifi_connect();
    }
    else if( WIFI_EVENT_STA_DISCONNECTED == event_id )
    {
      if( wifi_connect_retry < WIFI_MAX_RETRY )
      {
        uint32_t delay = (1 << wifi_connect_retry) * WIFI_CONNECT_DELAY;
        delay = (delay > WIFI_MAX_DELAY) ? WIFI_MAX_DELAY : delay;
        // waiting for some time before retrying again
        vTaskDelay(delay / portTICK_PERIOD_MS);
        esp_wifi_connect();
        wifi_connect_retry++;
        ESP_LOGI(TAG, "Retry Wi-Fi connection (%d/%d)...", wifi_connect_retry, WIFI_MAX_RETRY);
      }
      else
      {
        wifi_connect_status = false;
        xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
        ESP_LOGE(TAG, "Failed to connect to Access Point.");
      }
    }
  } // if( WIFI_EVENT = event_base )
  else if( IP_EVENT == event_base )
  {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
    wifi_connect_retry = 0;
    wifi_connect_status = true;
    xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

