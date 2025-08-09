/*
 * app_mqtt.c
 *
 *  Created on: Aug 8, 2025
 *      Author: xpress_embedo
 */

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "mqtt_client.h"
#include "gui_mng.h"
#include "mqtt_app.h"

// Private Macros
#define MQTT_APP_QUEUE_SIZE                           (5)
#define MQTT_APP_TASK_SIZE                            (4*1024u)
#define MQTT_APP_TASK_PRIORITY                        (4u)

// Private Variables
static const char *TAG = "APP_MQTT";
// Queue handle used to manipulate the main queue events
static QueueHandle_t mqtt_app_q_handle;

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
static void mqtt_app_task(void *pvParameter);
static void mqtt_app_mng( void );
static void mqtt_event_handler(void *args, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void mqtt_handle_app_data(esp_mqtt_event_handle_t event);

// Public Function Definitions
/**
 * @brief MQTT application start
 * @param  none
 */
void mqtt_app_start( void )
{
  ESP_LOGI( TAG, "Starting MQTT App." );

  // create a message queue
  mqtt_app_q_handle = xQueueCreate( MQTT_APP_QUEUE_SIZE, sizeof(mqtt_app_q_msg_t) );

  // start the mqtt application task
  xTaskCreate(&mqtt_app_task, "mqtt app task", MQTT_APP_TASK_SIZE, NULL, MQTT_APP_TASK_PRIORITY, NULL);
}

BaseType_t mqtt_app_send_msg( mqtt_app_msg_e msg_id )
{
  mqtt_app_q_msg_t msg;
  msg.msg_id = msg_id;
  return xQueueSend( mqtt_app_q_handle, &msg, portMAX_DELAY );
}

static void mqtt_app_task( void *pvParameter )
{
  mqtt_app_q_msg_t msg;

  for( ;; )
  {
    // wait for 1 second and then proceed
    if( xQueueReceive( mqtt_app_q_handle, &msg, pdMS_TO_TICKS(1000)) )
    {
      switch( msg.msg_id )
      {
        case MQTT_APP_MSG_START_CONNECTION:
          ESP_LOGI( TAG, "MQTT_APP_MSG_START_CONNECTION" );

          // configure MQTT host/broker related stuff here
          esp_mqtt_client_config_t mqtt_cfg =
          {
            .broker.address.uri = "mqtt://test.mosquitto.org:1883",
          };

          mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
          // The last argument may be used to pass data to the event handler, in this example mqtt_event_handler
          esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
          esp_mqtt_client_start(mqtt_client);

          break;
        case MQTT_APP_MSG_STOP_CONNECTION:
          ESP_LOGI( TAG, "MQTT_APP_MSG_STOP_CONNECTION" );
          if( mqtt_client )
          {
            esp_mqtt_client_stop(mqtt_client);     // Stop the client and disconnect from the broker
            esp_mqtt_client_destroy(mqtt_client);  // Free resources associated with the client
            mqtt_client = NULL;
          }
          break;
        case MQTT_APP_MSG_PUBLISH_XYZ:
          ESP_LOGI( TAG, "MQTT_APP_MSG_PUBLISH_XYZ" );
          break;
      }
    }

    // this function should be called every 1 second, in future to be removed
    mqtt_app_mng();
  }
}

// Private Function Definitions
static void mqtt_app_mng( void )
{
  // this is a patch work and needs improvement in future
  // not doing this for now, as the plan is to change the implementation
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
static void mqtt_handle_app_data(esp_mqtt_event_handle_t event)
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
      mqtt_handle_app_data( event );
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
