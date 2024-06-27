#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "driver/gpio.h"
#include <time.h>
#include <sys/time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_sntp.h"

#include "main.h"
#include "influxDB.h"

// macros
#define MAIN_TASK_PERIOD                    (60000)
#define APP_WIFI_SSID                       CONFIG_ESP_WIFI_SSID
#define APP_WIFI_PSWD                       CONFIG_ESP_WIFI_PASSWORD
#define WIFI_MAX_RETRY                      (5)
#define WIFI_CONNECT_DELAY                  (500)     // Initial delay in milliseconds
#define WIFI_MAX_DELAY                      (60000)   // Maximum delay in milliseconds
// The following are the bits/flags for event group
#define WIFI_CONNECTED_BIT                  BIT0      // connected to the access point with an IP
#define WIFI_FAIL_BIT                       BIT1      // failed to connect after the max. amount of retries

// Private Variables
static const char *TAG = "APP";
/* Sensor Related Variables */
static uint8_t temperature = 20;
static uint8_t humidity = 50;
static bool increasing = 1;
/* WiFi Connection Related Variables */
static EventGroupHandle_t wifi_event_group;           // FreeRTOS event group to signal when we are connected
static uint8_t wifi_connect_retry = 0;
static bool wifi_connect_status = false;
static bool sntp_connect_status = false;

// Private Function Declarations
static void app_connect_wifi( void );
static void wifi_event_handler( void *arg, esp_event_base_t event_base, int32_t event_id, void * event_data );
static void app_sntp_init( void );
static bool app_sntp_get_time( void );

void app_main(void)
{
  // Disable default gpio logging messages
  esp_log_level_set("gpio", ESP_LOG_NONE);
  // disable default wifi logging messages
  esp_log_level_set("wifi", ESP_LOG_NONE);
  esp_log_level_set("wifi_init", ESP_LOG_NONE);
  esp_log_level_set("sleep", ESP_LOG_NONE);
  esp_log_level_set("spi_flash", ESP_LOG_NONE);
  vTaskDelay(1000 / portTICK_PERIOD_MS);

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
    ESP_LOGI( TAG, "WiFi Connected, now synchronizing with NTP server." );
    app_sntp_init();
    sntp_connect_status = app_sntp_get_time();
    // if time fetched then only start the influxDB server
    if( sntp_connect_status )
    {
      // now start the influxDB task
      influxdb_start();
    }
  }

  while(1)
  {
    // simulating temperature and humidity
    if( increasing )
    {
      if (temperature <= 40)
      {
        temperature++;
        humidity++;
      }
      else
      {
        increasing = 0;   // change the direction to decreasing
      }
    }
    else
    {
      if( temperature >= 20 )
      {
        temperature--;
        humidity--;
      }
      else
      {
        increasing = 1;   // change the direction to increasing
      }
    }

    ESP_LOGI( TAG, "Temperature: %d, Humidity: %d\n", temperature, humidity);

    // if wifi is connected, trigger event to send data to InfluxDB cloud
    if( wifi_connect_status && sntp_connect_status )
    {
      influxdb_send_event(INFLUXDB_EV_TEMP_HUMID, NULL);
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
uint8_t get_temperature( void )
{
  return temperature;
}

uint8_t get_humidity( void )
{
  return humidity;
}

/**
 * @brief Get the MAC Address of the device
 * @param mac_str used to return the mac address as string
 */
void get_mac_address( char *mac_str )
{
  uint8_t mac[6];
  esp_read_mac( mac, ESP_MAC_WIFI_STA );
  snprintf( mac_str, MAC_ADDR_SIZE, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
}

long long get_time_ns( void )
{
  struct timeval now;
  gettimeofday( &now, NULL );
  long long time_ns = (long long)now.tv_sec * 1000000000LL + now.tv_usec * 1000LL;

  return time_ns;
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

  ESP_LOGI( TAG, "Connecting with WiFi Router" );

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

/**
 * @brief Initialize the SNTP
 * @param  None
 */
static void app_sntp_init( void )
{
  ESP_LOGI( TAG, "Initializing SNTP" );
  esp_sntp_setoperatingmode( SNTP_OPMODE_POLL );
  esp_sntp_setservername( 0, "pool.ntp.org" );  // set the SNTP server
  esp_sntp_init();
}

/**
 * @brief Synchronize the time from the SNTP server
 * @param  None
 * @return true if synchronization is successful else false
 */
static bool app_sntp_get_time( void )
{
  #define MAX_RETRY_COUNT_SNTP    (20)
  bool status = false;
  char time_buffer[50] = { 0 };   // temporary: only for printing/debugging

  // set the time zone to India Standard Time (IST)
  setenv( "TZ", "IST-5:30", 1);
  tzset();

  // wait for the time to be set
  time_t now = 0;
  struct tm time_info = { 0 };
  uint8_t retry = 0;

  time(&now);
  localtime_r( &now, &time_info );
  // the function is similar to snprintf, but is used to format the time
  strftime(time_buffer, sizeof(time_buffer), "%d.%m.%Y %H:%M:%S", &time_info);
  ESP_LOGI(TAG, "Sync Current Time: %s", time_buffer);

  while( (time_info.tm_year < (2020-1900)) && (retry < MAX_RETRY_COUNT_SNTP) )
  {
    ESP_LOGI( TAG, "Synchronizing the time.....%d", retry );
    retry++;
    vTaskDelay( retry*1000/portTICK_PERIOD_MS );
    time(&now);
    // The localtime_r() function converts the calendar time pointed to by clock
    // into a broken-down time stored in the structure to which result points.
    // The localtime_r() function also returns a pointer to that same structure.
    localtime_r( &now, &time_info );

    memset(time_buffer, 0x00, sizeof(time_buffer) );
    // the function is similar to snprintf, but is used to format the time
    strftime(time_buffer, sizeof(time_buffer), "%d.%m.%Y %H:%M:%S", &time_info);
    ESP_LOGI(TAG, "Sync Current Time: %s", time_buffer);
  }

  if( retry < MAX_RETRY_COUNT_SNTP )
  {
    // it means time is synchronized with SNTP server
    status = true;
  }
  return status;
}
