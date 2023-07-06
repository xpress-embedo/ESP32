#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "esp_system.h"
#include "esp_http_client.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <cJSON.h>

// Macros
#define WIFI_SUCCESS        				(0x01<<0u)
#define WIFI_FAILURE        				(0x01<<1u)
#define TCP_SUCCESS         				(0x01<<0u)
#define TCP_FAILURE         				(0x01<<1u)
#define MAX_FAILURES        				(10u)
#define HTTP_RESPONSE_SIZE  				(1024u)

// Structures
typedef struct _weather_data_t
{
  float temperature;
  int pressure;
  int humidity;
} weather_data_t;

// Private Variables
// Event Group to contain status information
static EventGroupHandle_t wifi_event_group;
static int retry_num = 0;
static const char *TAG = "WIFI";
static const char *CLIENT_KEY = "Content-Type";
static const char *CLIENT_VALUE = "application/x-www-form-urlencoded";
static const char *CLIENT_REQ_PRE = "https://api.openweathermap.org/data/2.5/weather?q=";
static const char *CLIENT_REQ_POST = "&APPID=ENTER_YOUR_KEY_HERE&units=metric";
static const char *city = "manali";
static weather_data_t city_weather;
char *response_data = NULL;
size_t response_len = 0;
bool all_data_received = false;

// Private Functions Prototypes
// Connect to WiFi and Return the Results
static esp_err_t connect_wifi(void);
// Event Handler for WiFi Events
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data);
// Event Handler for IP Events
static void ip_event_handler(void* arg, esp_event_base_t event_base,
                             int32_t event_id, void* event_data);
static void openweather_task(void *pvParameters);
static esp_err_t openweathermap_event_handler(esp_http_client_event_t *event);
static void openweathermap_get_weather(const char *json_string, weather_data_t *weather_data);

// Main Program Starts from Here
void app_main(void)
{
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Connect to Wireless Access Point
  ret = connect_wifi();
  if( ret )
  {
    ESP_LOGI(TAG, "Creating the Open Weather Map Task");
    xTaskCreate( &openweather_task,
                 "OpenWeatherMap",
                 8192, NULL, 6, NULL);
  }
  while (true)
  {
    printf("Hello from app_main!\n");
    sleep(1);
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
      .ssid = "ssid name",
      .password = "password",
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
    // ESP_LOGI(TAG, "STA IP: ", IPSTR, IP2STR(&event->ip_info.ip));
    ESP_LOGI(TAG, "STA IP: " IPSTR, IP2STR(&event->ip_info.ip));
    retry_num = 0;
    xEventGroupSetBits(wifi_event_group, WIFI_SUCCESS);
  }
}

// OpenWeatherMap Task
static void openweather_task(void *pvParameters)
{
  char openweathermap_url[200];
  snprintf( openweathermap_url, sizeof(openweathermap_url), \
            "%s%s%s", CLIENT_REQ_PRE, city, CLIENT_REQ_POST);

  esp_http_client_config_t config =
  {
    .url = openweathermap_url,
    .method = HTTP_METHOD_GET,
    .event_handler = openweathermap_event_handler,
  };

  esp_http_client_handle_t client = esp_http_client_init(&config);
  esp_http_client_set_header(client, CLIENT_KEY, CLIENT_VALUE);
  esp_err_t err = esp_http_client_perform(client);
  if( err == ESP_OK )
  {
    int status = esp_http_client_get_status_code(client);
    if(status == 200)
    {
      ESP_LOGI(TAG, "Message Sent Successfully");
    }
    else
    {
      ESP_LOGI(TAG, "Message Sent Failed");
    }
  }
  else
  {
    ESP_LOGI(TAG, "Message Sent Failed");
  }
  esp_http_client_cleanup(client);
  vTaskDelete(NULL);
}

static esp_err_t openweathermap_event_handler(esp_http_client_event_t *event)
{
  switch(event->event_id)
  {
    case HTTP_EVENT_ON_DATA:
      // Resize the Buffer to fit the new chunk of the data
      response_data = realloc(response_data, response_len + event->data_len);
      // Copy the Data
      memcpy(response_data+response_len, event->data, event->data_len);
      // Update the Length
      response_len += event->data_len;
      ESP_LOGI("OpenWeatherAPI", "Partial Received Data: %d", event->data_len);
      break;
    case HTTP_EVENT_ON_FINISH:
      all_data_received = true;
      // NOTE: TAG is different here
      ESP_LOGI("OpenWeatherAPI", "Received Data: %s", response_data);
      // Decode/Parse the weather data from the response data
      openweathermap_get_weather(response_data, &city_weather);
      // free up the space
      free(response_data);
      ESP_LOGI("OpenWeatherAPI", "Temp=%f, Pressure=%d, Humidity=%d", city_weather.temperature, city_weather.pressure, city_weather.humidity);
      break;
    default:
      break;
  }
  return ESP_OK;
}

static void openweathermap_get_weather(const char *json_string, weather_data_t *weather_data)
{
  cJSON *root = cJSON_Parse(json_string);
  cJSON *obj = cJSON_GetObjectItemCaseSensitive(root, "main");
  weather_data->temperature = cJSON_GetObjectItemCaseSensitive(obj, "temp")->valuedouble;
  weather_data->pressure = cJSON_GetObjectItemCaseSensitive(obj, "pressure")->valueint;
  weather_data->humidity = cJSON_GetObjectItemCaseSensitive(obj, "humidity")->valueint;
  cJSON_Delete(root);
}
