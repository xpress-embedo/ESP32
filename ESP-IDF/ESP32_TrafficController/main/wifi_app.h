/*
 * wifi_app.h
 *
 *  Created on: Aug 8, 2025
 *      Author: xpress_embedo
 */

#ifndef MAIN_WIFI_APP_H_
#define MAIN_WIFI_APP_H_

#include "esp_netif.h"

// Public Macros
#define WIFI_AP_SSID                    "ESP32_AP"
#define WIFI_AP_PASSWORD                "12345678"
#define WIFI_AP_CHANNEL                 1
#define WIFI_AP_SSID_HIDDEN             0               // Access Point Visibility
#define WIFI_AP_MAX_CONNECTIONS         5               // Access Point Max. Connection
/* Beacon Broadcast interval is the time lag between each of the beacons sent by
 * your router or access points (ESP32 in our case). By Definition the lower the
 * value, the smaller the time lag which means beacon is sent more frequently.
 * The higher the value, the bigger the time lag which means that the beacon is
 * sent broadcasted less frequently.
 * The beacon is need for your devices or clients to receive information about
 * the particular router information such as SSID, Timestamp & various parameters
 */
#define WIFI_AP_BEACON_INTERVAL         100             // 100ms which is by default also
#define WIFI_AP_IP                      "192.168.0.1"   // default IP address
#define WIFI_AP_GATEWAY                 "192.168.0.1"
#define WIFI_AP_NETMASK                 "255.255.255.0"
#define WIFI_AP_BANDWIDTH               WIFI_BW_HT20    // AP Bandwidth 20MHz (40MHz is other option)
#define WIFI_STA_POWER_SAVE             WIFI_PS_NONE    // No Power Save
#define WIFI_MAX_SSID_LENGTH            32
#define WIFI_MAX_PASSWORD_LENGTH        64
#define WIFI_MAX_CONNECTION_RETRIES     5

/*
 * Message IDs for the WiFi Application Task
 */
typedef enum _wifi_app_msg_e
{
  WIFI_APP_MSG_START_HTTP_SERVER = 0,
  WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER,
  WIFI_APP_MSG_STA_CONNECTED_GOT_IP,
  WIFI_APP_MSG_USR_REQUESTED_STA_DISCONNECT,
  WIFI_APP_MSG_LOAD_SAVED_CREDENTIALS,
  WIFI_APP_MSG_STA_DISCONNECTED,
} wifi_app_msg_e;

/*
 * Structure for Message Queue
 */
typedef struct _wifi_app_queue_msg
{
  wifi_app_msg_e msg_id;
} wifi_app_queue_msg_t;


// Global Variables Declaration
// netif objects for the station mode and access point modes
extern esp_netif_t* esp_netif_sta;
extern esp_netif_t* esp_netif_ap;

// Public Function Declarations
void wifi_app_start( void );
BaseType_t wifi_app_send_msg( wifi_app_msg_e msg_id );
wifi_config_t * wifi_app_get_wifi_config( void );

#endif /* MAIN_WIFI_APP_H_ */
