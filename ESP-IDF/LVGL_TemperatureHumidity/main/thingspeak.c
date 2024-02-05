/*
 * thingspeak.c
 *
 *  Created on: Feb 5, 2024
 *      Author: xpress_embedo
 */
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "esp_wifi.h"
#include "esp_http_client.h"

#include "main.h"
#include "thingspeak.h"

// Private Macros
#define WIFI_SSID
#define WIFI_PASSWORD
#define WIFI_MAX_RETRY                      (5)
#define THINGSPEAK_EVENT_QUEUE_LEN          (5)

// The following are the bits/flags for event group
#define WIFI_CONNECTED_BIT                  BIT0      // connected to the access point with an IP
#define WIFI_FAIL_BIT                       BIT1      // failed to connect after the max. amount of retries

// Private Variables
static const char *TAG = "ThingSpeak";
static const char THINGSPEAK_KEY[] = "Enter Your Key";
static EventGroupHandle_t wifi_event_group;           // FreeRTOS event group to signal when we are connected
static uint8_t wifi_connect_retry = 0;
static bool wifi_connect_status = false;
static QueueHandle_t      thingspeak_event = NULL;

// Private Function Declaration
static void thingspeak_connect_wifi( void );
static void wifi_event_handler( void *arg, esp_event_base_t event_base, int32_t event_id, void * event_data );
static void thingspeak_send_data( void *pvParameters );


// Public Function Prototypes
void thingspeak_start( void )
{
  // disable default wifi logging messages
  esp_log_level_set("wifi", ESP_LOG_NONE);

  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  thingspeak_connect_wifi();

  if( wifi_connect_status )
  {
    // create message queue with the length THINGSPEAK_EVENT_QUEUE_LEN
    thingspeak_event = xQueueCreate( THINGSPEAK_EVENT_QUEUE_LEN, sizeof(thingspeak_q_msg_t) );
    if( thingspeak_event == NULL )
    {
      ESP_LOGE(TAG, "Unable to Create Queue");
    }
    xTaskCreate(&thingspeak_send_data, "ThingSpeak Send", 4096*2, NULL, 6, NULL);
  }
}


BaseType_t thingspeak_send_event( thingspeak_event_t event, uint8_t *pData )
{
  BaseType_t status = pdFALSE;
  thingspeak_q_msg_t msg;

  if( event < THING_SPEAK_EV_MAX )
  {
    msg.event_id  = event;
    msg.data      = pData;
    status = xQueueSend( thingspeak_event, &msg, portMAX_DELAY );
  }
  return status;
}


static void thingspeak_connect_wifi( void )
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
      .ssid = WIFI_SSID,
      .password = WIFI_PASSWORD,
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
    ESP_LOGI(TAG, "Connected to Access Point %s", WIFI_SSID );
  }
  else if( bits & WIFI_FAIL_BIT )
  {
    ESP_LOGE(TAG, "Failed to Connect to Access Point %s", WIFI_SSID );
  }
  else
  {
    ESP_LOGE(TAG, "Unexpected Event" );
  }
  vEventGroupDelete(wifi_event_group);
}

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
        // waiting for some time before retrying again
        // todo: there shall be an setting for this
        vTaskDelay(pdMS_TO_TICKS(100));
        esp_wifi_connect();
        wifi_connect_retry++;
        ESP_LOGI(TAG, "Retrying to connect to Access Point." );
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

static void thingspeak_send_data( void *pvParameters )
{
  esp_err_t err;
  thingspeak_q_msg_t msg;
  char thingspeak_url[] = "https://api.thingspeak.com";
  char data[] = "/update?api_key=%s&field1=%u&field2=%d";
  char post_data[200];
  uint8_t temperature = 0;
  uint8_t humidity = 0;

  esp_http_client_config_t config =
  {
    .url = thingspeak_url,
    .method = HTTP_METHOD_GET,
  };

  esp_http_client_handle_t client = esp_http_client_init( &config );
  esp_http_client_set_header( client, "Content-Type", "application/x-www-form-urlencoded" );

  while( 1 )
  {
    // wait only 10 ms and then proceed
    if( xQueueReceive(thingspeak_event, &msg, pdMS_TO_TICKS(10)) )
    {
      // the below is the code to handle the state machine
      if( THING_SPEAK_EV_NONE != msg.event_id )
      {
        switch( msg.event_id )
        {
          case THING_SPEAK_EV_TEMP_HUMID:
            sensor_data_t *sensor_data = get_temperature_humidity();
            size_t idx = sensor_data->sensor_idx;
            if( (idx > 0) && (idx < SENSOR_BUFF_SIZE) )
            {
              // before posting the event we have incremented the index and hence to get
              // the last sensor data we have to use - 1
              idx = (idx - 1);
              temperature = sensor_data->temperature[idx];
              humidity = sensor_data->humidity[idx];
            }
            else
            {
              temperature = 0;
              humidity = 0;
            }
            strcpy(post_data,"");
            snprintf(post_data, sizeof(post_data), data, THINGSPEAK_KEY, temperature, humidity);
            ESP_LOGI(TAG, "Post Data = %s", post_data);
            esp_http_client_set_post_field(client, post_data, strlen(post_data));
            err = esp_http_client_perform(client);
            if (err == ESP_OK)
            {
              int status_code = esp_http_client_get_status_code(client);
              if( status_code == 200 )
              {
                ESP_LOGI(TAG, "Message Sent Successfully.");
              }
              else
              {
                ESP_LOGE(TAG, "Message Sending Failed.");
              }
            }
            break;
          default:
            break;
        } // switch case end
      }   // if event received in limit end
    }     // xQueueReceive end
  }
}
