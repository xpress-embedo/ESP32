/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "esp_event.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_wifi.h"

#include "nvs_flash.h"
#include "mqtt_client.h"

#include "main.h"
#include "wifi_app.h"
#include "gui_mng.h"

// Private Macros
#define MAIN_TASK_PERIOD                    (1000)
// #define APP_WIFI_SSID                       "Enter WIFI SSID"
// #define APP_WIFI_PSWD                       "Enter WiFI Password"
#define APP_WIFI_SSID                       "SECOND"
#define APP_WIFI_PSWD                       "Brutal@garry"
#define WIFI_MAX_RETRY                      (100)
#define WIFI_CONNECT_DELAY                  (500)     // Initial delay in milliseconds
#define WIFI_MAX_DELAY                      (60000)   // Maximum delay in milliseconds
// The following are the bits/flags for event group
#define WIFI_CONNECTED_BIT                  BIT0      // connected to the access point with an IP
#define WIFI_FAIL_BIT                       BIT1      // failed to connect after the max. amount of retries

// Private Variables
static const char *TAG = "APP";
/* WiFi Connection Related Variables */
static EventGroupHandle_t wifi_event_group;   // FreeRTOS event group to signal when we are connected
static uint8_t wifi_connect_retry = 0;
static bool wifi_connect_status = false;
// mqtt client for global access to publish and subscribe
static esp_mqtt_client_handle_t mqtt_client;
static bool mqtt_connect_status = false;
static char * traffic_topic = "TrafficTopic";
static char * traffic_time_1_topic = "TrafficTimeSide1";
static char * traffic_time_2_topic = "TrafficTimeSide2";
static char * traffic_time_3_topic = "TrafficTimeSide3";
static char * traffic_time_4_topic = "TrafficTimeSide4";

static uint8_t traffic_led_1 = TRAFFIC_LED_INVALID;
static uint8_t traffic_led_2 = TRAFFIC_LED_INVALID;
static uint8_t traffic_led_3 = TRAFFIC_LED_INVALID;
static uint8_t traffic_led_4 = TRAFFIC_LED_INVALID;

static uint8_t traffic_time_side1 = 0;
static uint8_t traffic_time_side2 = 0;
static uint8_t traffic_time_side3 = 0;
static uint8_t traffic_time_side4 = 0;

// Private Function Declarations
static void app_connect_wifi( void );
static void mqtt_app_start( void );
static void wifi_event_handler( void *arg, esp_event_base_t event_base, int32_t event_id, void * event_data );
static void mqtt_event_handler(void *args, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void app_handle_mqtt_data(esp_mqtt_event_handle_t event);

void app_main(void)
{
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_LOGI(TAG, "Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
  ESP_LOGI(TAG, "IDF version: %s", esp_get_idf_version());

  esp_log_level_set("gpio", ESP_LOG_NONE);

  // start the GUI manager
  gui_start();

  // send an event to GUI manager
  gui_send_event(GUI_MNG_EV_WIFI_CONNECTING, NULL);

  wifi_app_start();

  // connect with WiFi router
  // app_connect_wifi();

  // connect with mqtt server if connection is successful
  if( wifi_connect_status )
  {
    // send an event to GUI manager
    gui_send_event(GUI_MNG_EV_MQTT_CONNECTING, NULL);
    mqtt_app_start();
  }

  while (true )
  {
    if( traffic_time_side1 )
    {
      traffic_time_side1--;
      gui_send_event( GUI_MNG_EV_TRAFFIC_TIME_1, &traffic_time_side1 );
    }

    if( traffic_time_side2 )
    {
      traffic_time_side2--;
      gui_send_event( GUI_MNG_EV_TRAFFIC_TIME_2, &traffic_time_side2 );
    }

    if( traffic_time_side3 )
    {
      traffic_time_side3--;
      gui_send_event( GUI_MNG_EV_TRAFFIC_TIME_3, &traffic_time_side3 );
    }

    if( traffic_time_side4 )
    {
      traffic_time_side4--;
      gui_send_event( GUI_MNG_EV_TRAFFIC_TIME_4, &traffic_time_side4 );
    }
    // Wait before next
    vTaskDelay(MAIN_TASK_PERIOD / portTICK_PERIOD_MS);
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
}

/**
 * @brief MQTT application start
 * @param  none
 */
static void mqtt_app_start( void )
{
  // configure MQTT host/broker related stuff here
  esp_mqtt_client_config_t mqtt_cfg =
  {
    .broker.address.uri = "mqtt://test.mosquitto.org:1883",
  };

  mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
  // The last argument may be used to pass data to the event handler, in this example mqtt_event_handler
  esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
  esp_mqtt_client_start(mqtt_client);
}

/**
 * @brief Function to handle all mqtt subscribtion
 * @param event pointer to event data this is a pointer check typedef
 * @note Simulate topics manually using the following publish command
 * mosquitto_pub -h test.mosquitto.org -p 1883 -t TrafficTopic -m "Hello World"
 * mosquitto_pub -h test.mosquitto.org -p 1883 -t TrafficTopic -m "GREEN1 RED2 RED3 RED4"
 * mosquitto_pub -h test.mosquitto.org -p 1883 -t TrafficTopic -m "YELLOW1 RED2 RED3 RED4"
 * mosquitto_pub -h test.mosquitto.org -p 1883 -t TrafficTopic -m "RED1 RED2 RED3 RED4"
 * And if needed to verification use the following subscribe command
 * mosquitto_sub -h test.mosquitto.org -p 1883 -t TrafficTopic
 */
static void app_handle_mqtt_data(esp_mqtt_event_handle_t event)
{
  printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
  printf("DATA=%.*s\r\n", event->data_len, event->data);
  printf("DATA Length= %d\r\n", event->data_len);

  char * topic = event->topic;
  char * data  = event->data;
  // Create a temporary buffer to hold the time string safely
  char time_str[10]; // Enough for seconds as string
  int copy_len;
  gui_mng_event_t gui_event = GUI_MNG_EV_MAX;

  // handle the subscribe topics here
  // note: here it is important to use strncmp function, rather than strcmp function
  // I am not sure, but it looks like that the null character was creating problem
  // hence I used strncmp function and check the bytes excluding the last null
  if (event->topic_len == strlen(traffic_topic) && strncmp(topic, traffic_topic, event->topic_len) == 0)
  {
    // Preparing Events for Side-1
    if( strstr( data, "GREEN1") != NULL )
    {
      gui_event = GUI_MNG_EV_TRAFFIC_LED_1;
      traffic_led_1 = TRAFFIC_LED_GREEN;
    }
    else if( strstr( data, "YELLOW1") != NULL )
    {
      gui_event = GUI_MNG_EV_TRAFFIC_LED_1;
      traffic_led_1 = TRAFFIC_LED_YELLOW;
    }
    else if( strstr( data, "RED1") != NULL )
    {
      gui_event = GUI_MNG_EV_TRAFFIC_LED_1;
      traffic_led_1 = TRAFFIC_LED_RED;
    }
    // send event
    if( gui_event != GUI_MNG_EV_MAX )
    {
      gui_send_event( gui_event, &traffic_led_1 );
    }

    // Preparing Events for Side-2
    gui_event = GUI_MNG_EV_MAX;
    if( strstr( data, "GREEN2") != NULL )
    {
      gui_event = GUI_MNG_EV_TRAFFIC_LED_2;
      traffic_led_2 = TRAFFIC_LED_GREEN;
    }
    else if( strstr( data, "YELLOW2") != NULL )
    {
      gui_event = GUI_MNG_EV_TRAFFIC_LED_2;
      traffic_led_2 = TRAFFIC_LED_YELLOW;
    }
    else if( strstr( data, "RED2") != NULL )
    {
      gui_event = GUI_MNG_EV_TRAFFIC_LED_2;
      traffic_led_2 = TRAFFIC_LED_RED;
    }
    // send event
    if( gui_event != GUI_MNG_EV_MAX )
    {
      gui_send_event( gui_event, &traffic_led_2 );
    }

    // Preparing Events for Side-3
    gui_event = GUI_MNG_EV_MAX;
    if( strstr( data, "GREEN3") != NULL )
    {
      gui_event = GUI_MNG_EV_TRAFFIC_LED_3;
      traffic_led_3 = TRAFFIC_LED_GREEN;
    }
    else if( strstr( data, "YELLOW3") != NULL )
    {
      gui_event = GUI_MNG_EV_TRAFFIC_LED_3;
      traffic_led_3 = TRAFFIC_LED_YELLOW;
    }
    else if( strstr( data, "RED3") != NULL )
    {
      gui_event = GUI_MNG_EV_TRAFFIC_LED_3;
      traffic_led_3 = TRAFFIC_LED_RED;
    }
    // send event
    if( gui_event != GUI_MNG_EV_MAX )
    {
      gui_send_event( gui_event, &traffic_led_3 );
    }

    // Preparing Events for Side-4
    gui_event = GUI_MNG_EV_MAX;
    if( strstr( data, "GREEN4") != NULL )
    {
      gui_event = GUI_MNG_EV_TRAFFIC_LED_4;
      traffic_led_4 = TRAFFIC_LED_GREEN;
    }
    else if( strstr( data, "YELLOW4") != NULL )
    {
      gui_event = GUI_MNG_EV_TRAFFIC_LED_4;
      traffic_led_4 = TRAFFIC_LED_YELLOW;
    }
    else if( strstr( data, "RED4") != NULL )
    {
      gui_event = GUI_MNG_EV_TRAFFIC_LED_4;
      traffic_led_4 = TRAFFIC_LED_RED;
    }
    // send event
    if( gui_event != GUI_MNG_EV_MAX )
    {
      gui_send_event( gui_event, &traffic_led_4 );
    }
  }
  // add for any other topic
  else if (event->topic_len == strlen(traffic_time_1_topic) && strncmp(topic, traffic_time_1_topic, event->topic_len) == 0)
  {
    memset(time_str, 0, sizeof(time_str));
    // Copy the data from event->data
    copy_len = (event->data_len < sizeof(time_str) - 1) ? event->data_len : sizeof(time_str) - 1;
    memcpy(time_str, event->data, copy_len);

    // Convert string to integer
    traffic_time_side1 = atoi(time_str);
    gui_send_event( GUI_MNG_EV_TRAFFIC_TIME_1, &traffic_time_side1 );
  }
  else if (event->topic_len == strlen(traffic_time_2_topic) && strncmp(topic, traffic_time_2_topic, event->topic_len) == 0)
  {
    memset(time_str, 0, sizeof(time_str));
    // Copy the data from event->data
    copy_len = (event->data_len < sizeof(time_str) - 1) ? event->data_len : sizeof(time_str) - 1;
    memcpy(time_str, event->data, copy_len);

    // Convert string to integer
    traffic_time_side2 = atoi(time_str);
    gui_send_event( GUI_MNG_EV_TRAFFIC_TIME_2, &traffic_time_side2 );
  }
  else if (event->topic_len == strlen(traffic_time_3_topic) && strncmp(topic, traffic_time_3_topic, event->topic_len) == 0)
  {
    memset(time_str, 0, sizeof(time_str));
    copy_len = (event->data_len < sizeof(time_str) - 1) ? event->data_len : sizeof(time_str) - 1;
    memcpy(time_str, event->data, copy_len);

    // Convert string to integer
    traffic_time_side3 = atoi(time_str);
    gui_send_event( GUI_MNG_EV_TRAFFIC_TIME_3, &traffic_time_side3 );
  }
  else if (event->topic_len == strlen(traffic_time_4_topic) && strncmp(topic, traffic_time_4_topic, event->topic_len) == 0)
  {
    memset(time_str, 0, sizeof(time_str));
    // Copy the data from event->data
    copy_len = (event->data_len < sizeof(time_str) - 1) ? event->data_len : sizeof(time_str) - 1;
    memcpy(time_str, event->data, copy_len);

    // Convert string to integer
    traffic_time_side4 = atoi(time_str);
    gui_send_event( GUI_MNG_EV_TRAFFIC_TIME_4, &traffic_time_side4 );
  }
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
      wifi_connect_status = false;
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
 * @brief MQTT Event Handler Function
 * @param arg
 * @param event_base Event Base MQTT Event
 * @param event_id   Event ID
 * @param event_data Data with Event
 */
static void mqtt_event_handler(void *args, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
  ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", event_base, event_id);
  esp_mqtt_event_handle_t event = event_data;
  esp_mqtt_client_handle_t client = event->client;
  int msg_id;

  switch ( (esp_mqtt_event_id_t)event_id )
  {
    case MQTT_EVENT_CONNECTED:
      mqtt_connect_status = true;
      ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

      // Subscribe to Traffic Data Topic
      msg_id = esp_mqtt_client_subscribe(client, traffic_topic, 0);
      ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
      // Subscribe to Traffic Time for Side-1 Topic
      msg_id = esp_mqtt_client_subscribe(client, traffic_time_1_topic, 0);
      ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
      // Subscribe to Traffic Time for Side-2 Topic
      msg_id = esp_mqtt_client_subscribe(client, traffic_time_2_topic, 0);
      ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
      // Subscribe to Traffic Time for Side-3 Topic
      msg_id = esp_mqtt_client_subscribe(client, traffic_time_3_topic, 0);
      ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
      // Subscribe to Traffic Time for Side-4 Topic
      msg_id = esp_mqtt_client_subscribe(client, traffic_time_4_topic, 0);
      ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

      // send an event to GUI manager that we are connected
      gui_send_event(GUI_MNG_EV_MQTT_CONNECTED, NULL);
      break;
    case MQTT_EVENT_DISCONNECTED:
      ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
      mqtt_connect_status = false;
      break;
    case MQTT_EVENT_SUBSCRIBED:
      ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
      break;
    case MQTT_EVENT_UNSUBSCRIBED:
      ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
      break;
    case MQTT_EVENT_PUBLISHED:
      ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
      break;
    case MQTT_EVENT_DATA:
      ESP_LOGI(TAG, "MQTT_EVENT_DATA");
      // this function handles all the mqtt related topics and data
      app_handle_mqtt_data( event );
      break;
    case MQTT_EVENT_ERROR:
      ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
      if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
      {
        /*
        log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
        log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
        log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
        ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        */
      }
      break;
    default:
      ESP_LOGI(TAG, "Other event id:%d", event->event_id);
      break;
  }
}
