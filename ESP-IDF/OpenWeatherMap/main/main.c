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
#define MAX_FAILURES        	      (10u)
#define CITY_NAME_LEN               (10u)
#define NUM_OF_CITIES  				      (4u)
#define HTTP_RESP_LEN               (1024u)
#define MAIN_TASK_EXEC_RATE         (5000u)
#define HTTP_REQ_EXEC_RATE          (5000u)   // 3 seconds

// Structures
typedef struct _weather_data_t
{
  char city_name[CITY_NAME_LEN];   // assuming city name will not be more than CITY_NAME_LEN bytes
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
// static const char *CLIENT_REQ_POST = "&APPID=ENTER_YOUR_KEY_HERE&units=metric";
static const char *CLIENT_REQ_POST = "&APPID=fbd756d6387c660e650b533ff585c70e&units=metric";
static weather_data_t city_weather[NUM_OF_CITIES];
static uint8_t city_weather_index = 0;

char response_data[HTTP_RESP_LEN] = {0};
uint16_t response_data_idx = 0u;
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
static void openweathermap_task(void *pvParameters);
static void openweathermap_send_request(void);
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

  // Giving some delay at power-up
  vTaskDelay(MAIN_TASK_EXEC_RATE / portTICK_PERIOD_MS);

  // Connect to Wireless Access Point
  ret = connect_wifi();
  if( ret == WIFI_SUCCESS )
  {
    ESP_LOGI(TAG, "Creating the Open Weather Map Task");
    xTaskCreate( &openweathermap_task,
                 "OpenWeatherMap",
                 8192, NULL, 6, NULL);
    while (true)
    {
      // ESP_LOGI(TAG, "Hello World from Main Task");
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
      // .ssid = "ssid name",
      // .password = "password",
      .ssid = "gigacube-D733",
      .password = "5yR8m2B6Y4egrJQ5",
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

// OpenWeatherMap Task
static void openweathermap_task(void *pvParameters)
{
  // Initialize the City Names
  strcpy(city_weather[0].city_name, "manali");
  strcpy(city_weather[1].city_name, "shimla");
  strcpy(city_weather[2].city_name, "jaipur");
  strcpy(city_weather[3].city_name, "leh");
  
  for( ; ; )
  {
    openweathermap_send_request();
    vTaskDelay(HTTP_REQ_EXEC_RATE / portTICK_PERIOD_MS);
  }

  vTaskDelete(NULL);
}

static void openweathermap_send_request(void)
{
  char openweathermap_url[200];
  snprintf( openweathermap_url, sizeof(openweathermap_url), \
            "%s%s%s", CLIENT_REQ_PRE, city_weather[city_weather_index].city_name, CLIENT_REQ_POST);

  esp_http_client_config_t config =
  {
    .url = openweathermap_url,
    .method = HTTP_METHOD_GET,
    .event_handler = openweathermap_event_handler,
  };

  // ESP_LOGI("OpenWeatherMap", "URL:%s", openweathermap_url);

  esp_http_client_handle_t client = esp_http_client_init(&config);
  esp_http_client_set_header(client, CLIENT_KEY, CLIENT_VALUE);
  esp_err_t err = esp_http_client_perform(client);
  if( err == ESP_OK )
  {
    int status = esp_http_client_get_status_code(client);
    if(status == 200)
    {
      ESP_LOGI(TAG, "City=%s, Message Sent Successfully", city_weather[city_weather_index].city_name);
      city_weather_index++;
      // Reset back to Initial Position
      if( city_weather_index >= NUM_OF_CITIES )
      {
        city_weather_index = 0;
      }
    }
    else
    {
      ESP_LOGI(TAG, "City=%s, Message Sent Failed", city_weather[city_weather_index].city_name);
    }
  }
  else
  {
    ESP_LOGI(TAG, "City=%s, Message Sent Failed", city_weather[city_weather_index].city_name);
  }
  esp_http_client_cleanup(client);
}

static esp_err_t openweathermap_event_handler(esp_http_client_event_t *event)
{
  switch(event->event_id)
  {
    case HTTP_EVENT_ON_DATA:
      // Copy the Data to response_data buffer
      memcpy(response_data+response_data_idx, event->data, event->data_len);
      // Update the Length
      response_data_idx += event->data_len;
      // Only used for debugging
      ESP_LOGI("OpenWeatherAPI", "Received Data Size: %d, Total Data: %d", event->data_len, response_data_idx);
      break;
    case HTTP_EVENT_ON_FINISH:
      all_data_received = true;
      // Decode/Parse the weather data from the response data
      openweathermap_get_weather(response_data, &city_weather[city_weather_index]);
      // reset the response buffer and also the length to initial state
      memset(response_data, 0x00, sizeof(response_data));
      response_data_idx = 0;
      ESP_LOGI("OpenWeatherAPI", "City=%s, Temp=%f, Pressure=%d, Humidity=%d", \
                city_weather[city_weather_index].city_name,   \
                city_weather[city_weather_index].temperature, \
                city_weather[city_weather_index].pressure,    \
                city_weather[city_weather_index].humidity);
      ESP_LOGI("OpenWeatherAPI", "Exiting Event Handler");
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
