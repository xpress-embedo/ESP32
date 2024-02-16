#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_timer.h"

#include "gui_mng.h"
#include "time.h"
#include "sntp_time_sync.h"

// Private Macros
#define APP_WIFI_SSID                       "Enter WIFI SSID"
#define APP_WIFI_PSWD                       "Enter WiFI Password"
#define WIFI_MAX_RETRY                      (5)
#define WIFI_CONNECT_DELAY                  (500)     // Initial delay in milliseconds
#define WIFI_MAX_DELAY                      (60000)   // Maximum delay in milliseconds
// The following are the bits/flags for event group
#define WIFI_CONNECTED_BIT                  BIT0      // connected to the access point with an IP
#define WIFI_FAIL_BIT                       BIT1      // failed to connect after the max. amount of retries

#define MAIN_TASK_TIME                      (1000u)
#define SNTP_TIME_SYNC                      (1000u*60u/MAIN_TASK_TIME)   // synchronize every 60seconds with NTP server
#define TOD_INCREMENT_PERIOD_MS             (1000u)   // for local timer, to increment Time of Day

// Private Variables
static const char *TAG = "APP";
/* WiFi Connection Related Variables */
static EventGroupHandle_t wifi_event_group;   // FreeRTOS event group to signal when we are connected
static uint8_t wifi_connect_retry = 0;
static bool wifi_connect_status = false;
static bool first_time_sync = false;          // variable to track if time is sync
static uint8_t time_sync_counter = 0;         // counter to synchronize with NTP server
static struct tm time_info = { 0 };           // local copy of time information, this gets updated every second
                                              // using micro-controller clock, and every minutes using NTP server

// Private Function Declarations
static void app_connect_wifi( void );
static void tod_increment( void *arg );
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

  // start the GUI manager
  gui_start();

  while (true)
  {
    if( wifi_connect_status == false )
    {
      // wifi not connected, connect with WiFi (it will take some time)
      app_connect_wifi();
      if( wifi_connect_status )
      {
        gui_send_event(GUI_MNG_EV_WIFI_CONNECTED, (uint8_t*)&time_info );
        ESP_LOGI(TAG, "WiFi connected, now synchronizing with NTP server.");

        // connection establish, start the SNTP task
        sntp_time_sync_start();
      }
    }
    else
    {
      // check if synchronization with NTP server is cone or not
      if( first_time_sync == false )
      {
        sntp_time_sync_get_time_tm( &time_info );
        if( time_info.tm_year > (2016-1900) )
        {
          gui_send_event(GUI_MNG_EV_SNTP_SYNC, NULL);
          ESP_LOGI(TAG, "Time Synchronization done move to next screen.");
          first_time_sync = true;
          // now start a periodic timer to update the clock every second
          // Tod Update timer
          const esp_timer_create_args_t tod_increment_timer_args =
          {
            .callback = &tod_increment,
            .name = "ToD Increment"
          };
          esp_timer_handle_t tod_increment_timer;
          ESP_ERROR_CHECK(esp_timer_create(&tod_increment_timer_args, &tod_increment_timer));
          ESP_ERROR_CHECK(esp_timer_start_periodic(tod_increment_timer, TOD_INCREMENT_PERIOD_MS * 1000));  // here time is in micro seconds
        }
        else
        {
          ESP_LOGI(TAG, "Time not synchronize, will try again..");
        }
      }
      else
      {
        time_sync_counter++;
        if( time_sync_counter > SNTP_TIME_SYNC )
        {
          time_sync_counter = 0;
          ESP_LOGI(TAG, "Synchronizing Time");
          sntp_time_sync_get_time_tm( &time_info );
        }
      }
      // common delay for both scenarios
      vTaskDelay(MAIN_TASK_TIME/portTICK_PERIOD_MS);
    }
  }
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
 * @brief Increment Time of Day every second
 *        In this function second value is updated and event is posted to gui
 *        manager, so that it can update seconds dial
 * @param arg 
 */
static void tod_increment( void *arg )
{
  time_info.tm_sec++;
  // this is temporary alignment, in actual this will be updated with ntp sync
  if( time_info.tm_sec >= 60 )
  {
    time_info.tm_sec = 0;
    time_info.tm_min++;
  }
  if( time_info.tm_min >= 60 )
  {
    time_info.tm_min = 0;
    time_info.tm_hour++;
  }
  if( time_info.tm_hour >= 24 )
  {
    time_info.tm_hour = 0;
  }
  gui_send_event(GUI_MNG_EV_TIME_UPDATE, (uint8_t*)&time_info );
  // ESP_LOGI(TAG, "1sec Event to update display");
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

