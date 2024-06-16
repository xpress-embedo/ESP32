#include <lvgl.h>
#include <WiFi.h>
#include <DHT.h>
#include <DHT_U.h>
#include <PubSubClient.h>

#include "main.h"
#include "display_mng.h"

// Macros
#define DHTPIN                              (12)
#define DHTTYPE                             (DHT11)
#define LVGL_REFRESH_TIME                   (5u)      // 5 milliseconds
#define WIFI_MAX_SSID                       (7u)
#define DHT_REFRESH_TIME                    (20000u)   // 20 seconds
#define MQTT_PUB_BUFF_SIZE                  (20u)
#define WIFI_NOT_CONNECT_COUNT_MAX          (10)

// Private Variables
// Task Time Related
static uint32_t lvgl_refresh_timestamp = 0u;
static uint32_t dht_refresh_timestamp = 0u;
// Wi-Fi available access points drop down list
static char wifi_dd_list[WIFI_MAX_SSID*20] = { 0 };
// DHT11 Sensor Related
DHT dht(DHTPIN, DHTTYPE);

// MQTT Related
const char* mqtt_server = "hairdresser.cloudmqtt.com";  // MQTT Server Name
const int mqtt_port = 17259;                            // MQTT Server Port
const char* user_name = "pyptiouq";                     // MQTT Server Instance User Name
const char* mqtt_pswd = "aQp113ENJeO9";                 // MQTT Server Instance Password
WiFiClient esp_client;
PubSubClient client( esp_client );
char mqtt_pub_msg[MQTT_PUB_BUFF_SIZE] = { 0 };

// Private functions
static void LVGL_TaskInit( void );
static void LVGL_TaskMng( void );

void setup()
{
  Serial.begin(115200);

  LVGL_TaskInit();
  Display_Init();
}

void loop()
{
  Display_Mng();
  LVGL_TaskMng();
}

/**
 * @brief This function returns the WiFi SSID Names appended together which is 
 *        suitable for LVGL drop down widget
 * @param  none
 * @return pointer to wifi ssid drop down list
 */
char *Get_WiFiSSID_DD_List( void )
{
  return wifi_dd_list;
}

/**
 * @brief Initialize the WiFi mode to station mode and disconnect if connected
 * @param  none
 */
void WiFi_Init( void )
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
}

/**
 * @brief Scans for the SSID and store the results in a WiFi drop down list
 * "wifi_dd_list", this list is suitable to be used with the LVGL Drop Down
 * @param  none
 */
void WiFi_ScanSSID( void )
{
  String ssid_name;
  Serial.println("Start Scanning");
  int n = WiFi.scanNetworks();
  Serial.println("Scanning Done");
  if( n == 0 )
  {
    Serial.println("No Networks Found");
  }
  else
  {
    // I am restricting n to max WIFI_MAX_SSID value
    n = n <= WIFI_MAX_SSID ? n : WIFI_MAX_SSID;
    for (int i = 0; i < n; i++) 
    {
      if( i == 0 )
      {
        ssid_name = WiFi.SSID(i);
      }
      else
      {
        ssid_name = ssid_name + WiFi.SSID(i);
      }
      ssid_name = ssid_name + '\n';
      delay(10);
    }
    // clear the array, it might be possible that we coming after rescanning
    memset( wifi_dd_list, 0x00, sizeof(wifi_dd_list) );
    strcpy( wifi_dd_list, ssid_name.c_str() );
    Serial.println(wifi_dd_list);
  }
  Serial.println("Scanning Completed");
}

// Private Function Definitions
static void LVGL_TaskInit( void )
{
  lv_init();
  lvgl_refresh_timestamp = millis();
}

static void LVGL_TaskMng( void )
{
  uint32_t now = millis();
  // LVGL Refresh Timed Task
  if( (now - lvgl_refresh_timestamp) >= LVGL_REFRESH_TIME )
  {
    lvgl_refresh_timestamp = now;
    // let the GUI does work
    lv_timer_handler();
  }
}