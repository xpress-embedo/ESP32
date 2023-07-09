/*
 * openweathermap.c
 *
 *  Created on: 07-Jul-2023
 *      Author: xpress_embedo
 */

#include "openweathermap.h"
#include <cJSON.h>

// Macros
#define HTTP_RESP_LEN               (1024u)
#define HTTP_REQ_EXEC_RATE          (10000u)    // 10 seconds
#define CITY_NAME_LEN               (10u)
#define NUM_OF_CITIES               (4u)

// Structures
typedef struct _weather_data_t
{
  char city_name[CITY_NAME_LEN];   // assuming city name will not be more than CITY_NAME_LEN bytes
  int temperature;
  int pressure;
  int humidity;
} weather_data_t;

// Private Variables
static const char *TAG = "OpenWeatherMap";
static const char *CLIENT_KEY = "Content-Type";
static const char *CLIENT_VALUE = "application/x-www-form-urlencoded";
static const char *CLIENT_REQ_PRE = "https://api.openweathermap.org/data/2.5/weather?q=";
static const char *CLIENT_REQ_POST = "&APPID=ENTER_YOUR_KEY_HERE&units=metric";

static weather_data_t city_weather[NUM_OF_CITIES];
static uint8_t city_weather_index = 0;
char response_data[HTTP_RESP_LEN] = {0};
uint16_t response_data_idx = 0u;
bool request_in_process = false;

// Private Function Prototypes
static void openweathermap_send_request(void);
static esp_err_t openweathermap_event_handler(esp_http_client_event_t *event);
static void openweathermap_reset_buffer(void);
static void openweathermap_get_weather(const char *json_string, weather_data_t *weather_data);

// Public Function Definitions
void openweathermap_init(void)
{
  // Initialize the City Names
  strcpy(city_weather[0].city_name, "delhi");
  strcpy(city_weather[1].city_name, "shimla");
  strcpy(city_weather[2].city_name, "jaipur");
  strcpy(city_weather[3].city_name, "leh");
}

// OpenWeatherMap Manager
void openweathermap_mng(void)
{
  if( request_in_process == false )
  {
    request_in_process = true;
    openweathermap_send_request();
  }
}

// OpenWeatherMap Task (optional, if we don't want to use above two functions manually)
void openweathermap_task(void *pvParameters)
{
  openweathermap_init();
  for(;;)
  {
    openweathermap_mng();
    vTaskDelay(HTTP_REQ_EXEC_RATE/portTICK_PERIOD_MS);

  }
  vTaskDelete(NULL);
}

uint8_t openweathermap_get_numofcity(void)
{
  return NUM_OF_CITIES;
}

char* openweathermap_get_city_name(uint8_t city_idx)
{
  char *city_ptr = NULL;
  if( city_idx < NUM_OF_CITIES )
  {
    city_ptr = city_weather[city_idx].city_name;
  }
  return city_ptr;
}

int openweathermap_get_temperature(uint8_t city_idx)
{
  int temperature = 0;
  if( city_idx < NUM_OF_CITIES )
  {
    temperature = city_weather[city_idx].temperature;
  }
  return temperature;
}

int openweathermap_get_pressure(uint8_t city_idx)
{
  int pressure = 0;
  if( city_idx < NUM_OF_CITIES )
  {
    pressure = city_weather[city_idx].pressure;
  }
  return pressure;
}

int openweathermap_get_humidity(uint8_t city_idx)
{
  int humidity = 0;
  if( city_idx < NUM_OF_CITIES )
  {
    humidity = city_weather[city_idx].humidity;
  }
  return humidity;
}


// Private Function Definitions
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

  // ESP_LOGI(TAG, "URL:%s", openweathermap_url);
  ESP_LOGI(TAG, "Free Heap: %lu", esp_get_free_heap_size() );

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
      break;
    case HTTP_EVENT_ON_FINISH:
      // Decode/Parse the weather data from the response data
      openweathermap_get_weather(response_data, &city_weather[city_weather_index]);
      // reset the response buffer and also the length to initial state
      openweathermap_reset_buffer();
      ESP_LOGI( TAG, "City=%s, Temp=%d, Pressure=%d, Humidity=%d", \
                city_weather[city_weather_index].city_name,   \
                city_weather[city_weather_index].temperature, \
                city_weather[city_weather_index].pressure,    \
                city_weather[city_weather_index].humidity);
      city_weather_index++;
      // Reset back to Initial Position
      if( city_weather_index >= NUM_OF_CITIES )
      {
        city_weather_index = 0;
      }
      // Free the system for next requests
      request_in_process = false;
      break;
    case HTTP_EVENT_ERROR:
      // In case of Error, exit
      openweathermap_reset_buffer();
      // Free the system for next requests
      request_in_process = false;
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
  weather_data->temperature = cJSON_GetObjectItemCaseSensitive(obj, "temp")->valueint;
  weather_data->pressure = cJSON_GetObjectItemCaseSensitive(obj, "pressure")->valueint;
  weather_data->humidity = cJSON_GetObjectItemCaseSensitive(obj, "humidity")->valueint;
  cJSON_Delete(root);
}

static void openweathermap_reset_buffer(void)
{
  // reset the response buffer and also the length to initial state
  memset(response_data, 0x00, sizeof(response_data));
  response_data_idx = 0;
}

