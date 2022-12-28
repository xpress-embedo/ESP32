/*
 * wifi_app.c
 *
 *  Created on: 28-Dec-2022
 *      Author: xpress_embedo
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "lwip/netdb.h"

#include "rgb_led.h"
#include "tasks_common.h"
#include "wifi_app.h"

// Tag used for ESP Serial console messages
//static const char TAG[] = "WiFi APP";

// Queue Handle used to Manipulate the Main Queue of events
static QueueHandle_t wifi_app_queue_handle;

// Network Interface object for the station and access point
esp_netif_t* esp_netif_sta = NULL;
esp_netif_t* esp_netif_ap = NULL;

// Private function Declaration
static BaseType_t WiFi_App_SendMsg( WiFi_App_Msg_e msg_id );
static void WiFi_App_Task(void *pvParameters);
static void WiFi_App_EventHandlerInit( void );
static void WiFi_App_DefaultWiFi_Init( void );
static void WiFi_App_SoftAP_Config( void );
static void WiFi_App_EventHandler( void *arg, \
                                   esp_event_base_t event_base, \
                                   int32_t event_id, \
                                   void *event_data );

// Public Function Definitions
/*
 * Starts the WiFi RTOS Task
 */
void WiFi_App_Start( void )
{
//  ESP_LOGI( TAG, "Starting WiFi Application");
  // Start WiFi Started LED
  RGB_WiFi_Started();

  // Disable default WiFi Logging message, else too many msgs will be printed
  esp_log_level_set("wifi", ESP_LOG_NONE );

  // Create Message Queue
  wifi_app_queue_handle = xQueueCreate( WIFI_APP_QUEUE_SIZE, \
                                        sizeof(WiFi_App_Queue_Msg_s) );

  // Create Task
  xTaskCreatePinnedToCore( &WiFi_App_Task, "WiFi App Task", \
                           WIFI_APP_TASK_STACK_SIZE, NULL, \
                           WIFI_APP_TASK_PRIORIIRY, NULL, \
                           WIFI_APP_TASK_CORE_ID );
}


// Private Function Definition
/*
 * Main Task for the WiFi Application
 * @param pvParameters Parameters which can be passed to the task
 */
static void WiFi_App_Task(void *pvParameters)
{
  WiFi_App_Queue_Msg_s msg;

  // Initialize the Event Handler
  WiFi_App_EventHandlerInit();

  // Initialize the TCP/IP stack and WiFi Configuration
  WiFi_App_DefaultWiFi_Init();

  // SoftAP config
  WiFi_App_SoftAP_Config();

  // Start WiFi
  ESP_ERROR_CHECK( esp_wifi_start() );

  // Send the first Event message
  WiFi_App_SendMsg( WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER );

  for( ;; )
  {
    if( xQueueReceive(wifi_app_queue_handle, &msg, portMAX_DELAY) )
    {
      switch( msg.msg_id )
      {
//        case WIFI_APP_MSG_START_HTTP_SERVER:
//          ESP_LOGI( TAG, "WIFI_APP_MSG_START_HTTP_SERVER" );
//          // Start HTTP Server TODO
//          RGB_HTTP_ServerStarted();
//          break;
//        case WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER:
//          ESP_LOGI( TAG, "WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER");
//          break;
//        case WIFI_APP_MSG_STA_CONNECTED_GOT_IP:
//          ESP_LOGI( TAG, "WIFI_APP_MSG_STA_CONNECTED_GOT_IP");
//          break;
        default:
          break;
      }
    }
  }
}

/*
 * Sends a message to the FreeRTOS Queue
 * @param msg_id message ID from the WiFi_App_Msg_e enum
 * @return pdTRUE if an item was successfully sent to queue, otherwise pdFALSE.
 * @note expand the parameter list based on your requirements.
 */
BaseType_t WiFi_App_SendMsg( WiFi_App_Msg_e msg_id )
{
  WiFi_App_Queue_Msg_s msg;
  msg.msg_id = msg_id;
  return (xQueueSend(wifi_app_queue_handle, &msg, portMAX_DELAY) );
}

/*
 * Initializes the WiFi Application Event Handler for WiFi and IP Events
 */
static void WiFi_App_EventHandlerInit( void )
{
  // Event Loop for WiFi Driver
  ESP_ERROR_CHECK( esp_event_loop_create_default() );

  // Event Handler for Connection
  esp_event_handler_instance_t instance_wifi_event;
  esp_event_handler_instance_t instance_ip_event;
  esp_event_handler_instance_register( WIFI_EVENT, ESP_EVENT_ANY_ID, \
                                       &WiFi_App_EventHandler, NULL, \
                                       &instance_wifi_event );
  esp_event_handler_instance_register( IP_EVENT, ESP_EVENT_ANY_ID, \
                                       &WiFi_App_EventHandler, NULL, \
                                       &instance_ip_event );
}

/*
 * Initializes the TCP/IP Stack and default WiFi Configuration
 */
static void WiFi_App_DefaultWiFi_Init( void )
{
  // Initializes the TCP stack
  ESP_ERROR_CHECK( esp_netif_init() );

  // Default WiFi config - operations must be in this order only
  wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&wifi_init_config) );
  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );

  // initialize network interface objects
  esp_netif_sta = esp_netif_create_default_wifi_sta();
  esp_netif_ap = esp_netif_create_default_wifi_ap();
}

/*
 * Configures the WiFi Access Point Settings and Assigns the static IP Address
 * to the SoftAP
 */
static void WiFi_App_SoftAP_Config( void )
{
  // Soft Access Point - WiFi Access Point Configuration
  wifi_config_t ap_config =
  {
    .ap =
    {
      .ssid = WIFI_AP_SSID,
      .ssid_len = strlen(WIFI_AP_SSID),
      .password = WIFI_AP_PASSWORD,
      .channel = WIFI_AP_CHANNEL,
      .ssid_hidden = WIFI_AP_SSID_HIDDEN,
      .authmode = WIFI_AUTH_WPA2_PSK,
      .max_connection = WIFI_AP_MAX_CONNECTIONS,
      .beacon_interval = WIFI_AP_BEACON_INTERVAL,
    },
  };

  // Configure DHCP for the AP
  esp_netif_ip_info_t ap_ip_info;
  memset( &ap_ip_info, 0x00, sizeof(ap_ip_info) );

  // must be called first
  esp_netif_dhcps_stop(esp_netif_ap);

  // Assign access point's static IP, Gateway and Netmask
  // This function converts an Internet address in it's standard text format,
  // into it's numeric binary form
  inet_pton( AF_INET, WIFI_AP_IP, &ap_ip_info.ip );
  inet_pton( AF_INET, WIFI_AP_GATEWAY, &ap_ip_info.gw );
  inet_pton( AF_INET, WIFI_AP_NETMASK, &ap_ip_info.netmask );

  // Statically configure the network interface
  ESP_ERROR_CHECK( esp_netif_set_ip_info(esp_netif_ap, &ap_ip_info) );
  // Starts the AP DHCP Server
  ESP_ERROR_CHECK( esp_netif_dhcps_start(esp_netif_ap) );

  // Setting the mode as Access Point/Station Mode
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_APSTA) );

  // Set our configuration
  ESP_ERROR_CHECK( esp_wifi_set_config( ESP_IF_WIFI_AP, &ap_config) );

  // Our default bandwidth 20 MHz
  ESP_ERROR_CHECK( esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_AP_BANDWIDTH) );

  // Power Save set to None
  ESP_ERROR_CHECK( esp_wifi_set_ps( WIFI_STA_POWER_SAVE) );
}

/*
 * WiFi Application Event Handler
 * @param arg data, aside from the event data that is passed to the handler
 *                  when it is called.
 * @param event_base the base id of the event to register the handler for
 * @param event_data event data
 */
static void WiFi_App_EventHandler( void *arg, \
                                   esp_event_base_t event_base, \
                                   int32_t event_id, \
                                   void *event_data )
{
  if( event_base == WIFI_EVENT )
  {
//    switch( event_id )
//    {
//      case WIFI_EVENT_AP_START:
//        ESP_LOGI( TAG, "WIFI_EVENT_AP_START" );
//        break;
//      case WIFI_EVENT_AP_STOP:
//        ESP_LOGI( TAG, "WIFI_EVENT_AP_STOP" );
//        break;
//      case WIFI_EVENT_AP_STACONNECTED:
//        ESP_LOGI( TAG, "WIFI_EVENT_AP_STACONNECTED" );
//        break;
//      case WIFI_EVENT_AP_STADISCONNECTED:
//        ESP_LOGI( TAG, "WIFI_EVENT_AP_STADISCONNECTED" );
//        break;
//      case WIFI_EVENT_STA_START:
//        ESP_LOGI( TAG, "WIFI_EVENT_STA_START" );
//        break;
//      case WIFI_EVENT_STA_CONNECTED:
//        ESP_LOGI( TAG, "WIFI_EVENT_STA_CONNECTED" );
//        break;
//      case WIFI_EVENT_STA_DISCONNECTED:
//        ESP_LOGI( TAG, "WIFI_EVENT_STA_DISCONNECTED" );
//        break;
//    };
  }
  else if( event_base == IP_EVENT )
  {
    switch( event_id )
    {
      case IP_EVENT_STA_GOT_IP:
//        ESP_LOGI( TAG, "IP_EVENT_STA_GOT_IP" );
        break;
    };
  }
}
