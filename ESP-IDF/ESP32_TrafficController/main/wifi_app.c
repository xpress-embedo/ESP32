/*
 * wifi_app.c
 *
 *  Created on: Aug 8, 2025
 *      Author: xpress_embedo
 */

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"

#include "lwip/netdb.h"

#include <string.h>

#include "wifi_app.h"
#include "http_server.h"

// Private Macros
#define WIFI_APP_QUEUE_SIZE                           (5)
#define WIFI_APP_TASK_SIZE                            (4*1024u)
#define WIFI_APP_TASK_PRIORITY                        (5u)

// Private Variables
static const char *TAG = "WiFi_APP";

// Queue handle used to manipulate the main queue events
static QueueHandle_t wifi_app_q_handle;

// used for returning the WiFi configuration
static wifi_config_t * wifi_config = NULL;
// used to track the number of retries when a connection attempt fails
static int g_retry_number;

// netif objects for the station mode and access point modes
esp_netif_t* esp_netif_sta = NULL;
esp_netif_t* esp_netif_ap = NULL;

// Private Function Prototypes
static void wifi_app_task(void *pvParameter);
static void wifi_app_event_handler_init( void );
static void wifi_app_default_wifi_init( void );
static void wifi_app_soft_ap_config( void );
static void wifi_app_connect_sta(void);
static void wifi_app_event_handler( void *arg, esp_event_base_t event_base, int32_t event_id, void * event_data );

// Public Function Definitions
void wifi_app_start( void )
{
  ESP_LOGI( TAG, "Starting WiFi Application" );

  // Disable default logging messages
  esp_log_level_set("wifi", ESP_LOG_NONE);

  // Allocate memory for the WiFi Configuration
  wifi_config = (wifi_config_t*)malloc( sizeof(wifi_config_t) );
  memset( wifi_config, 0x00, sizeof(wifi_config_t) );

  // create a message queue
  wifi_app_q_handle = xQueueCreate( WIFI_APP_QUEUE_SIZE, sizeof(wifi_app_queue_msg_t) );

  // start the WiFi application task
  xTaskCreate(&wifi_app_task, "wifi app task", WIFI_APP_TASK_SIZE, NULL, WIFI_APP_TASK_PRIORITY, NULL);
}

BaseType_t wifi_app_send_msg( wifi_app_msg_e msg_id )
{
  wifi_app_queue_msg_t msg;
  msg.msg_id = msg_id;
  return xQueueSend( wifi_app_q_handle, &msg, portMAX_DELAY );
}

/*
 * Get the WiFi Configuration
 */
wifi_config_t * wifi_app_get_wifi_config( void )
{
  return wifi_config;
}

// Private Function Definitions
static void wifi_app_task(void *pvParameter)
{
  wifi_app_queue_msg_t msg;

  // initialize the event handler
  wifi_app_event_handler_init();

  // initialize the TCP/IP stack and wifi config
  wifi_app_default_wifi_init();

  // SoftAP Config
  wifi_app_soft_ap_config();

  // start wifi
  ESP_ERROR_CHECK( esp_wifi_start() );

  // send the first message
  wifi_app_send_msg( WIFI_APP_MSG_START_HTTP_SERVER );

  for( ;; )
  {
    if( xQueueReceive(wifi_app_q_handle, &msg, portMAX_DELAY) )
    {
      switch( msg.msg_id )
      {
        case WIFI_APP_MSG_START_HTTP_SERVER:
          ESP_LOGI( TAG, "WIFI_APP_MSG_START_HTTP_SERVER" );
          http_server_start();
          break;
        case WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER:
          ESP_LOGI( TAG, "WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER" );

          // Attempt a Connection
          wifi_app_connect_sta();

          // set the current number of retries to zero
          g_retry_number = 0;

          // Let the HTTP Server knows about the connection attempt
          http_server_monitor_send_msg( HTTP_MSG_WIFI_CONNECT_INIT );
          break;
        case WIFI_APP_MSG_STA_CONNECTED_GOT_IP:
          ESP_LOGI( TAG, "WIFI_APP_MSG_STA_CONNECTED_GOT_IP" );

          // send message to http server that esp32 is connected as station
          http_server_monitor_send_msg( HTTP_MSG_WIFI_CONNECT_SUCCESS );
          break;
        case WIFI_APP_MSG_USR_REQUESTED_STA_DISCONNECT:
          ESP_LOGI(TAG, "WIFI_APP_MSG_USR_REQUESTED_STA_DISCONNECT");

          g_retry_number = MAX_CONNECTION_RETRIES;
          ESP_ERROR_CHECK( esp_wifi_disconnect() );
          break;
        case WIFI_APP_MSG_STA_DISCONNECTED:
          ESP_LOGI(TAG,"WIFI_APP_MSG_STA_DISCONNECTED");

          http_server_monitor_send_msg( HTTP_MSG_WIFI_CONNECT_FAIL );
          break;
        default:
          break;
      }
    }
  }
}

// initialize the wifi application event handler for WiFo and IP Events
static void wifi_app_event_handler_init( void )
{
  ESP_ERROR_CHECK( esp_event_loop_create_default() );

  esp_event_handler_instance_t instance_wifi_event;
  esp_event_handler_instance_t instance_ip_event;

  ESP_ERROR_CHECK( esp_event_handler_instance_register( WIFI_EVENT,               \
                                                        ESP_EVENT_ANY_ID,         \
                                                        &wifi_app_event_handler,  \
                                                        NULL,                     \
                                                        &instance_wifi_event) );
  ESP_ERROR_CHECK( esp_event_handler_instance_register( IP_EVENT,                 \
                                                        ESP_EVENT_ANY_ID,         \
                                                        &wifi_app_event_handler,  \
                                                        NULL,                     \
                                                        &instance_ip_event) );
}

static void wifi_app_default_wifi_init( void )
{
  // initialize TCP/IP stack
  ESP_ERROR_CHECK( esp_netif_init() );

  // default wifi config - operations must be in this order
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
  ESP_ERROR_CHECK( esp_wifi_set_storage( WIFI_STORAGE_RAM) );

  esp_netif_sta = esp_netif_create_default_wifi_sta();
  esp_netif_ap = esp_netif_create_default_wifi_ap();
}

static void wifi_app_soft_ap_config( void )
{
  // SoftAP- wifi access point configuration
  wifi_config_t ap_config =
  {
    .ap =
    {
      .ssid = WIFI_AP_SSID,
      .ssid_len = strlen(WIFI_AP_SSID),
      .password = WIFI_AP_PASSWORD,
      .max_connection = WIFI_AP_MAX_CONNECTIONS,
      .channel = WIFI_AP_CHANNEL,
      .ssid_hidden = WIFI_AP_SSID_HIDDEN,
      .authmode = WIFI_AUTH_WPA2_PSK,
      .beacon_interval = WIFI_AP_BEACON_INTERVAL,
    },
  };

  // Configure the DHCP for the AP
  esp_netif_ip_info_t ap_ip_info;
  memset(&ap_ip_info, 0x00, sizeof(ap_ip_info));

  // Stop the DHCP Server, this must be called first
  esp_netif_dhcps_stop(esp_netif_ap);

  // this function inet_pton converts the ip address in standard numeric form
  inet_pton(AF_INET, WIFI_AP_IP, &ap_ip_info.ip);       // Assign Access Point's Static IP, GW and NetMask
  inet_pton(AF_INET, WIFI_AP_GATEWAY, &ap_ip_info.gw);
  inet_pton(AF_INET, WIFI_AP_NETMASK, &ap_ip_info.netmask);

  // Statically Configures the network interface
  ESP_ERROR_CHECK(esp_netif_set_ip_info(esp_netif_ap, &ap_ip_info));
  // Start the AP DHCP Server (for connecting stations i.e. our mobile devices)
  ESP_ERROR_CHECK(esp_netif_dhcps_start(esp_netif_ap));

  // Set the mode as Access Point and Station Mode
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
  // Set our configuration
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));
  // our Default bandwidth is 20MHz
  ESP_ERROR_CHECK(esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_AP_BANDWIDTH));
  ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_STA_POWER_SAVE));
}

/*
 * Connects the ESP32 to an external access point using the updated station
 * configuration
 */
static void wifi_app_connect_sta(void)
{
  ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_app_get_wifi_config()) );
  ESP_ERROR_CHECK( esp_wifi_connect() );
}


/**
 * @brief WiFi Event Handler Function
 * @param arg data, aside from event data, that is passed to the handler when it is called
 * @param event_base Event Base whether WIFI Event or IP Event
 * @param event_id   Event ID
 * @param event_data Data with Event
 */
static void wifi_app_event_handler( void *arg, esp_event_base_t event_base, int32_t event_id, void * event_data )
{
  if( WIFI_EVENT == event_base )
  {
    switch( event_id )
    {
      case WIFI_EVENT_AP_START:
        ESP_LOGI( TAG, "WIFI_EVENT_AP_START");
        break;
      case WIFI_EVENT_AP_STOP:
        ESP_LOGI( TAG, "WIFI_EVENT_AP_STOP");
        break;
      case WIFI_EVENT_AP_STACONNECTED:
        ESP_LOGI( TAG, "WIFI_EVENT_AP_STACONNECTED");
        break;
      case WIFI_EVENT_AP_STADISCONNECTED:
        ESP_LOGI( TAG, "WIFI_EVENT_AP_STADISCONNECTED");
        break;
      case WIFI_EVENT_STA_START:
        ESP_LOGI( TAG, "WIFI_EVENT_STA_START");
        break;
      case WIFI_EVENT_STA_CONNECTED:
        ESP_LOGI( TAG, "WIFI_EVENT_STA_CONNECTED");
        break;
      case WIFI_EVENT_STA_DISCONNECTED:
        ESP_LOGI( TAG, "WIFI_EVENT_STA_DISCONNECTED");
        wifi_event_sta_disconnected_t *event = (wifi_event_sta_disconnected_t*)malloc(sizeof(wifi_event_sta_disconnected_t));
        *event = *((wifi_event_sta_disconnected_t*)event_data);
        ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED, Reason Code %d", event->reason);

        if( g_retry_number < MAX_CONNECTION_RETRIES )
        {
          esp_wifi_connect();
          g_retry_number++;
        }
        else
        {
          wifi_app_send_msg( WIFI_APP_MSG_STA_DISCONNECTED );
        }
        break;
    }
  } // if( WIFI_EVENT = event_base )
  else if( IP_EVENT == event_base )
  {
    switch( event_id )
    {
      case IP_EVENT_STA_GOT_IP:
        ESP_LOGI( TAG, "IP_EVENT_STA_GOT_IP");

        wifi_app_send_msg( WIFI_APP_MSG_STA_CONNECTED_GOT_IP );
        break;
    }
  }
}
