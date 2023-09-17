/*
 * wifi_app.c
 *
 *  Created on: 15-Jul-2023
 *      Author: xpress_embedo
 */

#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "lwip/netdb.h"

#include "led_mng.h"
#include "tasks_common.h"
#include "http_server.h"
#include "wifi_app.h"
#include "app_nvs.h"

// Macros
#define LED_WIFI_APP_STARTED()                  LED_RED_ON()
#define LED_WIFI_HTTP_SERVER_STARTED()          LED_GREEN_ON()
#define LED_WIFI_CONNECTED()                    LED_BLUE_ON()

// Private Variables
static const char TAG[] = "WIFI_APP";     // Used for ESP Serial console Message
static QueueHandle_t wifi_app_q_handle;   // Queue Handle used to Manipulate the main queue of events

// netif objects for station and access point modes
esp_netif_t* esp_netif_sta = NULL;
esp_netif_t* esp_netif_ap = NULL;

// used for returning the WiFi configuration
wifi_config_t * wifi_config = NULL;

// used to track the number of retries when a connection attempt fails
static int g_retry_number;

// WiFi application Event group handle and status bits
static EventGroupHandle_t wifi_app_event_group;
const int WIFI_APP_CONNECTING_USING_SAVED_CREDS_BIT   = BIT0;
const int WIFI_APP_CONNECTING_FROM_HTTP_SERVER_BIT    = BIT1;
const int WIFI_APP_USER_REQUESTED_STA_DISCONNECT_BIT  = BIT2;

// Private Function Definitions
static void wifi_app_task(void *pvParameter);
static void wifi_app_event_handler_init(void);
static void wifi_app_default_wifi_init(void);
static void wifi_app_soft_ap_config(void);
static void wifi_app_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void wifi_app_connect_sta(void);

// Public Function Definitions
/*
 * Sends a message to the queue.
 * @param msg_id message from wifi_app_msg_e enum
 * @return pdTRUE if an item was successfully sent to the queue, otherwise pdFALSE
 */
BaseType_t wifi_app_send_msg( wifi_app_msg_e msg_id )
{
  wifi_app_q_msg_t msg;
  msg.msg_id = msg_id;
  return xQueueSend(wifi_app_q_handle, &msg, portMAX_DELAY);
}

/*
 * Starts the WiFi RTOS Task
 */
void wifi_app_start( void )
{
  ESP_LOGI(TAG,"Starting WiFi Application");

  // Start the WiFi Led
  LED_WIFI_APP_STARTED();

  // Disable default wifi logging messages
  esp_log_level_set("wifi", ESP_LOG_NONE);

  // Allocate memory for the WiFi Configuration
  wifi_config = (wifi_config_t*)malloc(sizeof(wifi_config_t));
  memset(wifi_config, 0x00, sizeof(wifi_config_t) );

  // Create Message Queue with length 3
  wifi_app_q_handle = xQueueCreate( 3, sizeof(wifi_app_q_msg_t));

  // Create WiFi Application Event Group
  // wifi_app_event_group = xEventCreateGroup()

  // Start the WiFi Application Task
  xTaskCreate(&wifi_app_task, "wifi_app_task", WIFI_APP_TASK_STACK_SIZE, NULL, WIFI_APP_TASK_PRIORITY, NULL);
}

/*
 * Get the WiFi Configuration
 */
wifi_config_t * wifi_app_get_wifi_config( void )
{
  return wifi_config;
}

/*
 * Main task for the WiFi Application
 */
static void wifi_app_task(void *pvParameter)
{
  wifi_app_q_msg_t msg;
  EventBits_t eventBits;

  // Initialize the Event Handler
  wifi_app_event_handler_init();

  // Initialize the TCP/IP stack and WiFi Configuration
  wifi_app_default_wifi_init();

  // Soft AP Configuration
  wifi_app_soft_ap_config();

  // Start WiFi
  ESP_ERROR_CHECK(esp_wifi_start());

  // Send First Event Message
  // wifi_app_send_msg(WIFI_APP_MSG_START_HTTP_SERVER);
  // Load the saved credentials from the NVS and to do that send the message
  wifi_app_send_msg(WIFI_APP_MSG_LOAD_SAVED_CREDENTIALS);

  for(;;)
  {
    if(xQueueReceive(wifi_app_q_handle, &msg, portMAX_DELAY))
    {
      switch( msg.msg_id)
      {
        case WIFI_APP_MSG_START_HTTP_SERVER:
          ESP_LOGI(TAG,"WIFI_APP_MSG_START_HTTP_SERVER");
          http_server_start();
          LED_WIFI_HTTP_SERVER_STARTED();
          break;
        case WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER:
          ESP_LOGI(TAG,"WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER");

          xEventGroupSetBits(wifi_app_event_group, WIFI_APP_CONNECTING_FROM_HTTP_SERVER_BIT);

          // Attempt a Connection
          wifi_app_connect_sta();

          // set the current number of retries to zero
          g_retry_number = 0;

          // Let the HTTP Server knows about the connection attempt
          http_server_monitor_send_msg( HTTP_MSG_WIFI_CONNECT_INIT );
          break;
        case WIFI_APP_MSG_STA_CONNECTED_GOT_IP:
          ESP_LOGI(TAG,"WIFI_APP_MSG_STA_CONNECTED_GOT_IP");
          LED_WIFI_CONNECTED();
          // send message to http server that esp32 is connected as station
          http_server_monitor_send_msg( HTTP_MSG_WIFI_CONNECT_SUCCESS );

          eventBits = xEventGroupGetBits(wifi_app_event_group);
          // save Station credentials only when saving from the HTTP server
          if( eventBits & WIFI_APP_CONNECTING_USING_SAVED_CREDS_BIT )
          {
            xEventGroupClearBits(wifi_app_event_group, WIFI_APP_CONNECTING_USING_SAVED_CREDS_BIT);
          }
          else
          {
            // save credentials
            app_nvs_save_sta_creds();
          }

          if( eventBits & WIFI_APP_CONNECTING_FROM_HTTP_SERVER_BIT )
          {
            xEventGroupClearBits(wifi_app_event_group, WIFI_APP_CONNECTING_FROM_HTTP_SERVER_BIT);
          }
          break;
        case WIFI_APP_MSG_USR_REQUESTED_STA_DISCONNECTED:
          ESP_LOGI(TAG, "WIFI_APP_MSG_USR_REQUESTED_STA_DISCONNECTED");
          
          xEventGroupSetBits(wifi_app_event_group, WIFI_APP_USER_REQUESTED_STA_DISCONNECT_BIT);

          g_retry_number = WIFI_MAX_CONN_RETRIES;
          ESP_ERROR_CHECK( esp_wifi_disconnect() );

          // disconnected hence delete/clear the credentials also
          app_nvs_clear_sta_creds();
          break;
        case WIFI_APP_MSG_STA_DISCONNECTED:
          ESP_LOGI(TAG,"WIFI_APP_MSG_STA_DISCONNECTED");

          eventBits = xEventGroupGetBits(wifi_app_event_group);

          if( eventBits & WIFI_APP_CONNECTING_USING_SAVED_CREDS_BIT )
          {
            ESP_LOGI(TAG, "WIFI_APP_MSG_STA_DISCONNECTED: Attempt Using Saved Credentials");
            xEventGroupClearBits(wifi_app_event_group, WIFI_APP_CONNECTING_USING_SAVED_CREDS_BIT);
            app_nvs_clear_sta_creds();
          }
          else if( eventBits & WIFI_APP_CONNECTING_FROM_HTTP_SERVER_BIT )
          {
            ESP_LOGI(TAG, "WIFI_APP_MSG_STA_DISCONNECTED: Attempt Using HTTP Server");
            xEventGroupClearBits(wifi_app_event_group, WIFI_APP_CONNECTING_FROM_HTTP_SERVER_BIT);
            // send message to http server that esp32 is disconnected as station
            http_server_monitor_send_msg( HTTP_MSG_WIFI_CONNECT_FAIL );
          }
          else if( eventBits & WIFI_APP_USER_REQUESTED_STA_DISCONNECT_BIT )
          {
            ESP_LOGI(TAG, "WIFI_APP_MSG_STA_DISCONNECTED: User Requested Disconnection");
            xEventGroupClearBits(wifi_app_event_group, WIFI_APP_USER_REQUESTED_STA_DISCONNECT_BIT);
            // send message to http server that esp32 is disconnected as station
            http_server_monitor_send_msg( HTTP_MSG_WIFI_USER_DISCONNECT );
          }
          else
          {
            ESP_LOGI(TAG, "WIFI_APP_MSG_STA_DISCONNECTED: Attempt Failed check WiFi Access Point availability");
            // Adjust this case according to our needs (let's say retrying etc)
          }

          break;
        case WIFI_APP_MSG_LOAD_SAVED_CREDENTIALS:
          ESP_LOGI(TAG, "WIFI_APP_MSG_LOAD_SAVED_CREDENTIALS");

          if( app_nvs_load_sta_creds() )
          {
            ESP_LOGI(TAG, "Loaded Station Credentials");
            wifi_app_connect_sta();
            xEventGroupSetBits(wifi_app_event_group, WIFI_APP_CONNECTING_USING_SAVED_CREDS_BIT);
          }
          else
          {
            ESP_LOGI(TAG, "Unable to Load Station Credentials");
          }

          // Next is to start the web server
          wifi_app_send_msg(WIFI_APP_MSG_START_HTTP_SERVER);
          break;
        default:
          break;
      }
    }
  }
}

/*
 * Initializes the WiFi Application Event Handler for WiFi and IP Events
 */
static void wifi_app_event_handler_init(void)
{
  // Initialize the default ESP Event Loop
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  // Event handler for the connection
  esp_event_handler_instance_t wifi_handler_event_instance;
  esp_event_handler_instance_t ip_handler_event_instance;

  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,               \
                                                      ESP_EVENT_ANY_ID,         \
                                                      &wifi_app_event_handler,  \
                                                      NULL,                     \
                                                      &wifi_handler_event_instance));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,                 \
                                                      ESP_EVENT_ANY_ID,         \
                                                      &wifi_app_event_handler,  \
                                                      NULL,                     \
                                                      &ip_handler_event_instance));
}

/*
 * Initializes the TCP stack and default WiFi Configuration
 */
static void wifi_app_default_wifi_init(void)
{
  // Initialize the TCP Stack i.e. the ESP Network Interface
  ESP_ERROR_CHECK(esp_netif_init());

  // Setup WiFi Station with the Default WiFi Configuration
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

  esp_netif_sta = esp_netif_create_default_wifi_sta();
  esp_netif_ap = esp_netif_create_default_wifi_ap();
}

/*
 * Configures the WiFi access point settings and assigns the static IP to SoftAP
 */
static void wifi_app_soft_ap_config(void)
{
  // SoftAP - WiFi Access Point Configuration
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
    }
  };

  // Configure the DHCP for the AP
  esp_netif_ip_info_t ap_ip_info;
  memset(&ap_ip_info, 0x00, sizeof(ap_ip_info));

  // Stop the DHCP Server, this must be called first
  esp_netif_dhcps_stop(esp_netif_ap);

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
 * WiFi Application Event Handler
 * @param arg data, aside from event data, that is passed to the handler when it is called
 * @param event_base the base id of the event to register the handler for
 * @param event_id the id for the event to register the handler for
 * @param event_data event data
 */
static void wifi_app_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
  if(event_base == WIFI_EVENT)
  {
    switch(event_id)
    {
      case WIFI_EVENT_AP_START:
        ESP_LOGI(TAG, "WIFI_EVENT_AP_START");
        break;
      case WIFI_EVENT_AP_STOP:
        ESP_LOGI(TAG, "WIFI_EVENT_AP_STOP");
        break;
      case WIFI_EVENT_AP_STACONNECTED:
          ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
          break;
      case WIFI_EVENT_AP_STADISCONNECTED:
          ESP_LOGI(TAG, "WIFI_EVENT_AP_STADISCONNECTED");
          break;
      case WIFI_EVENT_STA_START:
          ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
          break;
      case WIFI_EVENT_STA_STOP:
          ESP_LOGI(TAG, "WIFI_EVENT_STA_STOP");
          break;
      case WIFI_EVENT_STA_DISCONNECTED:
          ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");

          wifi_event_sta_disconnected_t *event = (wifi_event_sta_disconnected_t*)malloc(sizeof(wifi_event_sta_disconnected_t));
          *event = *((wifi_event_sta_disconnected_t*)event_data);
          ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED, Reason Code %d", event->reason);

          if( g_retry_number < WIFI_MAX_CONN_RETRIES )
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
  }
  else if(event_base == IP_EVENT)
  {
    switch(event_id)
    {
      case IP_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");

        wifi_app_send_msg( WIFI_APP_MSG_STA_CONNECTED_GOT_IP );
        break;
    }
  }
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
