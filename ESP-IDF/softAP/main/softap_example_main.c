/*  WiFi softAP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

/* This Example use WiFi config. that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_WIFI_CHANNEL   CONFIG_ESP_WIFI_CHANNEL
#define EXAMPLE_MAX_STA_CONN       CONFIG_ESP_MAX_STA_CONN

static const char *TAG = "wifi softAP";

// Private Function Declaration
static void wifi_init_softap(void);
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data);


void app_main(void)
{
  //Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if( (ret==ESP_ERR_NVS_NO_FREE_PAGES) || (ret==ESP_ERR_NVS_NEW_VERSION_FOUND) )
  {
    ESP_ERROR_CHECK( nvs_flash_erase() );
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
  wifi_init_softap();
}

// Private Function Definitions
static void wifi_init_softap( void )
{
  /* Initialize the Network Interface or we can say initialize underlying TCP/IP
  stack, this function should be called exactly once, when the application
  starts */
  ESP_ERROR_CHECK( esp_netif_init() );

  // Create Default Event Loop
  ESP_ERROR_CHECK( esp_event_loop_create_default() );

  /* This creates default WIFI AP. In case of any init error this API aborts.
  The API creates esp_netif object with default WiFi access point config,
  attaches the netif to wifi and registers default wifi handlers.*/
  esp_netif_create_default_wifi_ap();

  /* Initialize WiFi Allocate resource for WiFi driver, such as WiFi control
  structure, RX/TX buffer, WiFi NVS structure etc. This WiFi also starts WiFi
  task.
  Note: This API must be called before all other WiFi API can be called.
  Note: Always use WIFI_INIT_CONFIG_DEFAULT macro to initialize the
  configuration to default values, this can guarantee all the fields get correct
  value when more fields are added into wifi_init_config_t in future release.
  If you want to set your own initial values, overwrite the default values which
  are set by WIFI_INIT_CONFIG_DEFAULT.
  Please be notified that the field 'magic' of wifi_init_config_t should always
  be WIFI_INIT_CONFIG_MAGIC! */
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

  // Register an instance of event handler to the default loop.
  ESP_ERROR_CHECK(esp_event_handler_instance_register( WIFI_EVENT,\
                                                       ESP_EVENT_ANY_ID,\
                                                       &wifi_event_handler,\
                                                       NULL,
                                                       NULL) );

  wifi_config_t wifi_config =
  {
    .ap =
    {
      // this data will come from SDK Config, Example Configuration section
      .ssid = EXAMPLE_ESP_WIFI_SSID,
      .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
      .channel = EXAMPLE_ESP_WIFI_CHANNEL,
      .password = EXAMPLE_ESP_WIFI_PASS,
      .max_connection = EXAMPLE_MAX_STA_CONN,
      .authmode = WIFI_AUTH_WPA_WPA2_PSK,
      .pmf_cfg =
      {
        .required = false,
      },
    },
  };

  // if no password is specified change the authenticate mode to open, this is
  // used for open networks
  if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0)
  {
    wifi_config.ap.authmode = WIFI_AUTH_OPEN;
  }

  // Set the operating mode, here we are setting the Access Point Mode
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP) );

  // Set the configuration of the ESP32 STA or AP
  ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_AP, &wifi_config) );
  /* Start WiFi according to current configuration If mode is WIFI_MODE_STA, it
  create station control block and start station If mode is WIFI_MODE_AP, it
  create soft-AP control block and start soft-AP If mode is WIFI_MODE_APSTA,
  it create soft-AP and station control block and start soft-AP and station */
  ESP_ERROR_CHECK( esp_wifi_start() );

  ESP_LOGI( TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d", \
            EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, \
            EXAMPLE_ESP_WIFI_CHANNEL);
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
  if (event_id == WIFI_EVENT_AP_STACONNECTED)
  {
    wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
    ESP_LOGI(TAG, "station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
  }
  else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
  {
    wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
    ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d", MAC2STR(event->mac), event->aid);
  }
}
