
#include "main.h"
#include "display_mng.h"
#include "openweathermap.h"

// Macros
#define WIFI_SUCCESS        						(0x01<<0u)
#define WIFI_FAILURE        						(0x01<<1u)
#define TCP_SUCCESS         						(0x01<<0u)
#define TCP_FAILURE         						(0x01<<1u)
#define MAX_FAILURES        	      		(10u)
#define MAIN_TASK_EXEC_RATE             (100u)

#define ESP_TIME_RATE_MSEC              (1000)

#define OPENWEATHERMAP_MNG_RATE_MSEC    (10000)   // 10seconds
#define OPENWEATHERMAP_MNG_EXEC_RATE    (OPENWEATHERMAP_MNG_RATE_MSEC*ESP_TIME_RATE_MSEC)

#define DISPLAY_MNG_RATE_MSEC           (1000)    // 1seconds
#define DISPLAY_MNG_EXEC_RATE           (DISPLAY_MNG_RATE_MSEC*ESP_TIME_RATE_MSEC)

// Private Variables
// Event Group to contain status information
static EventGroupHandle_t wifi_event_group;
static int retry_num = 0;
static const char *TAG = "WIFI";

int64_t openweathermap_timestamp = 0;
int64_t display_timestamp = 0;

// Private Functions Prototypes
// Connect to WiFi and Return the Results
static esp_err_t connect_wifi(void);
// Event Handler for WiFi Events
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data);
// Event Handler for IP Events
static void ip_event_handler(void* arg, esp_event_base_t event_base,
                             int32_t event_id, void* event_data);

// Main Program Starts from Here
void app_main(void)
{
  int64_t current_time = 0;
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Connect to Wireless Access Point
  ret = connect_wifi();
  if( ret == WIFI_SUCCESS )
  {
    // Initialize the OpenWeatherMap module
    openweathermap_init();
    // Initialize the Display Manager
    display_init();
    openweathermap_timestamp = esp_timer_get_time();
    display_timestamp = esp_timer_get_time();

    while( true )
    {
      current_time = esp_timer_get_time();

      // OpenWeatherMap Management
      if( (current_time - openweathermap_timestamp) > OPENWEATHERMAP_MNG_EXEC_RATE )
      {
        openweathermap_timestamp = current_time;
        openweathermap_mng();
      }

      // Display Management
      if( (current_time - display_timestamp) > DISPLAY_MNG_EXEC_RATE )
      {
       display_timestamp = current_time;
       display_mng();
      }
      vTaskDelay(MAIN_TASK_EXEC_RATE / portTICK_PERIOD_MS);
    }
  }
}

// Private Function Definitions
// Connect to WiFi and Return the Results
esp_err_t connect_wifi(void)
{
  esp_err_t status = WIFI_FAILURE;

  /* Initialize all things here */
  // Initialize the ESP Network Interface
  ESP_ERROR_CHECK(esp_netif_init());

  // Initialize the default ESP Event Lopp
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  // Create WiFi Station in the WiFi Driver
  esp_netif_create_default_wifi_sta();

  // Setup WiFi Station with the Default WiFi Configuration
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  /* Event Loop */
  wifi_event_group = xEventGroupCreate();

  esp_event_handler_instance_t wifi_handler_event_instance;
  esp_event_handler_instance_t got_ip_event_instance;

  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,         \
                                                      ESP_EVENT_ANY_ID,   \
                                                      &wifi_event_handler,\
                                                      NULL,               \
                                                      &wifi_handler_event_instance));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,           \
                                                      IP_EVENT_STA_GOT_IP,\
                                                      &ip_event_handler,  \
                                                      NULL,               \
                                                      &got_ip_event_instance));

  // Start the WiFi Driver
  wifi_config_t wifi_config =
  {
    .sta =
    {
      .ssid = "TestWiFi",
      .password = "12345678",
      .threshold.authmode = WIFI_AUTH_WPA2_PSK,
      .pmf_cfg =
      {
        .capable = true,
        .required = false
      },
    },
  };

  // Set the WiFi Controller to be a station
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

  // Set the WiFi Configuration
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

  // Start the WiFi Driver
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(TAG, "STA Initialization Complete");

  /* Now we will wait here */
  EventBits_t bits = xEventGroupWaitBits(wifi_event_group,            \
                                         WIFI_SUCCESS | WIFI_FAILURE, \
                                         pdFALSE, pdFALSE,            \
                                         portMAX_DELAY);

  /* xEventGroupWaitBits() returns the bits before the call returned, hence we
  can test which event actually happened */
  if( bits & WIFI_SUCCESS )
  {
    ESP_LOGI(TAG, "Connected To Access Point");
    status = WIFI_SUCCESS;
  }
  else if( bits & WIFI_FAILURE )
  {
    ESP_LOGI(TAG, "Failed to Connect to Access Point");
    status = WIFI_FAILURE;
  }
  else
  {
    // Here Logging Error and not the Information
    ESP_LOGE(TAG, "Unexpected Error");
    status = WIFI_FAILURE;
  }

  // The purpose of the events is done hence can be unregistered now
  ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, got_ip_event_instance));
  ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_handler_event_instance));
  vEventGroupDelete(wifi_event_group);

  return status;
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
  if((event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_STA_START))
  {
    ESP_LOGI(TAG, "Connecting to AP...");
    esp_wifi_connect();
  }
  else if((event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_STA_DISCONNECTED))
  {
    if( retry_num < MAX_FAILURES )
    {
      ESP_LOGI(TAG, "Reconnecting to AP...");
      esp_wifi_connect();
      retry_num++;
    }
    else
    {
      // Not Possible to Connect
      xEventGroupSetBits(wifi_event_group, WIFI_FAILURE);
    }
  }
}

// Event Handler for IP Events
static void ip_event_handler(void* arg, esp_event_base_t event_base,
                             int32_t event_id, void* event_data)
{
  if((event_base==IP_EVENT) && (event_id==IP_EVENT_STA_GOT_IP))
  {
    ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
    ESP_LOGI(TAG, "STA IP: " IPSTR, IP2STR(&event->ip_info.ip));
    retry_num = 0;
    xEventGroupSetBits(wifi_event_group, WIFI_SUCCESS);
  }
}

