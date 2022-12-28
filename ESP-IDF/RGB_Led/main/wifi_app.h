/*
 * wifi_app.h
 *
 *  Created on: 28-Dec-2022
 *      Author: xpress_embedo
 */

#ifndef MAIN_WIFI_APP_H_
#define MAIN_WIFI_APP_H_

#include "esp_netif.h"


// WiFi Application Settings
#define WIFI_AP_SSID                      "ESP32-AP"
#define WIFI_AP_PASSWORD                  "12345678"
#define WIFI_AP_CHANNEL                   (1)         // Access Point Channel
#define WIFI_AP_SSID_HIDDEN               (0)         // Access Point Visibility
#define WIFI_AP_MAX_CONNECTIONS           (2)         // AP Max Clients

/*Beacon Broadcast Interval is the time lag between each of the beacons sent by
your router or access points. By definition, the lower the value, the smaller
the time lag, which means the beacon is sent more frequently.
The beacon is needed for your devices or clients to receive information about
the particular router. In this case the beacon includes some main information
such as SSID, Timestamp and various parameters.
Most of the routers out of the box has the default beacon interval function set
at 100ms.*/
#define WIFI_AP_BEACON_INTERVAL           (100)       // AP beacon: 100ms
#define WIFI_AP_IP                        "192.168.0.1"   // default IP
#define WIFI_AP_GATEWAY                   "192.168.0.1"   // same as IP
#define WIFI_AP_NETMASK                   "255.255.255.0"

/* WiFi HT20/40
ESP32 support WiFi bandwidth HT20 or HT240 and does not support HT20/40 coexist.
`esp_wifi_set_bandwidth()` function can be used to change the default bandwidth
of station or AP. The default bandwidth for ESP32 station and AP is HT40.

In station mode, the actual bandwidth is firstly negotiated during the WiFi
Connection. It is HT40 only if both station and the connected AP supports HT20,
otherwise it is HT20. If the bandwidth of connected AP changes, the actual
bandwidth is negotiated again without WiFi disconnecting.

Similarly, in AP mode, the actual bandwidth is negotiated between AP and the
stations that connect to the AP. It is HT40 if the AP and one of the stations
supports HT40, otherwise it is HT20.

In station/AP coexist mode, the station/AP can configure HT20/40 separately.
If both station and AP are negotiated to HT40, the HT40 channel should be the
channel of station because the station always has higher priority than AP in
ESP32.
E.g. the configured bandwidth of AP is HT40, the configured primary channel is
6 and the configured secondary channel is 10. The station is connected to an
router whose primary channel is 6 and secondary channel is 2, then the actual
channel of AP is changed to primary 6 and secondary 2 automatically.

Theoretically the HT40 can gain better throughput because the maximum raw
physical (PHY) data rate for HT40 is 150Mbps while it’s 72Mbps for HT20.
However, if the device is used in some special environment, e.g. there are
too many other Wi-Fi devices around the ESP32 device, the performance of HT40
may be degraded. So if the applications need to support same or similar
scenarios, it’s recommended that the bandwidth is always configured to HT20.*/
#define WIFI_AP_BANDWIDTH                 WIFI_BW_HT20    // Bandwidth 20MHz
#define WIFI_STA_POWER_SAVE               WIFI_PS_NONE    // No Power Save
#define WIFI_MAX_SSID_LEN                 (32u)           // IEEE Standard Max.
#define WIFI_MAX_PASSWORD_LEN             (64u)           // IEEE Standard Max.
#define WIFI_MAX_CONNECTION_RETRY         (5u)

// Network Interface object for the station and access point
extern esp_netif_t* esp_netif_sta;
extern esp_netif_t* esp_netif_ap;

/*
 * Message IDs for the WiFi application task
 * @note Expand this based on your application requirements.
 */
typedef enum _WiFi_App_Msg_e
{
  WIFI_APP_MSG_START_HTTP_SERVER = 0,
  WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER,
  WIFI_APP_MSG_STA_CONNECTED_GOT_IP,
} WiFi_App_Msg_e;

/*
 * Structure for the message queue
 * @note Expand based on the application requirement
 */
typedef struct _WiFi_App_Queue_Msg_s
{
  WiFi_App_Msg_e msg_id;
} WiFi_App_Queue_Msg_s;

// Public Function Prototypes
void WiFi_App_Start( void );

#endif /* MAIN_WIFI_APP_H_ */
