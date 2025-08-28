/*
 * gui_mng_cfg.c
 *
 *  Created on: Jun 15, 2024
 *      Author: xpress_embedo
 */

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "main.h"
#include "ui.h"
#include "lvgl.h"
#include "gui_mng.h"
#include "ui_SensorScreen.h"
#include "gui_mng_cfg.h"

// Private Macros
#define NUM_ELEMENTS(x)                 (sizeof(x)/sizeof(x[0]))
#define LOAD_SENSOR_SCREEN_TIMER        ( 2000/ GUI_MNG_REFRESH_TIME )

// function template for callback function
typedef void (*gui_mng_callback)(uint8_t * data);

// structure
typedef struct _gui_mng_event_cb_t
{
  gui_mng_event_t   event;
  gui_mng_callback  callback;
} gui_mng_event_cb_t;

// Private Function Prototypes
static void gui_wifi_connecting( uint8_t *data );
static void gui_wifi_connected( uint8_t *data );
static void gui_wifi_internet_connected( uint8_t *data );
static void gui_wifi_disconnected( uint8_t *data );
static void gui_load_sensor_screen( uint8_t *data );
static void gui_update_sensor_data( uint8_t *data );

// Private Variables
static const char *TAG = "GUI_CFG";
static uint32_t load_sensor_screen_timer = 0;

static const gui_mng_event_cb_t gui_mng_event_cb[] =
{
  { GUI_MNG_EV_WIFI_CONNECTING,         gui_wifi_connecting           },
  { GUI_MNG_EV_WIFI_CONNECTED,          gui_wifi_connected            },
  { GUI_MNG_EV_WIFI_DISCONNECTED,       gui_wifi_disconnected         },
  { GUI_MNG_EV_WIFI_INTERNET_CONNECTED, gui_wifi_internet_connected   },
  { GUI_MNG_EV_LOAD_SENSOR_SCREEN,      gui_load_sensor_screen        },
  { GUI_MNG_EV_TEMP_HUMID,              gui_update_sensor_data        },
};

// Public Function Definitions
/**
 * @brief GUI Configurable Initialization Function
 * @param  None
 */
void gui_cfg_init( void )
{
  ESP_LOGI( TAG, "UI Init. Starts" );
  ui_init();
  ESP_LOGI( TAG, "UI Init. Ends" );
}

/**
 * @brief Process the events posted to GUI manager module
 *        This function calls the dedicated function based on the event posted
 *        to GUI manager queue, I will think of moving this function to GUI manager
 * @param event event name
 * @param data event data pointer
 */
void gui_cfg_mng_process( gui_mng_event_t event, uint8_t *data )
{
  uint8_t idx = 0;
  for( idx=0; idx < NUM_ELEMENTS(gui_mng_event_cb); idx++ )
  {
    // check if event matches the table
    if( event == gui_mng_event_cb[idx].event )
    {
      // call the callback function with arguments, if not NULL
      if( gui_mng_event_cb[idx].callback != NULL )
      {
        gui_mng_event_cb[idx].callback(data);
      }
    }
  }
}

/**
 * @brief this is a custom refresh function called periodically by GUI manager
 *        in this function we can write our code which can be called periodically
 */
void gui_cfg_refresh( void )
{
  if( (load_sensor_screen_timer > 0) && (--load_sensor_screen_timer == 0) )
  {
    gui_send_event( GUI_MNG_EV_LOAD_SENSOR_SCREEN, NULL );
  }
}

/**
 * @brief Callback function when ESP32 is connecting to WiFi router
 * @param data 
 */
static void gui_wifi_connecting( uint8_t *data )
{
  // update the connect icon status to disconnected
  lv_img_set_src( ui_imgWiFiStatus1,  &ui_img_wifi_disconnected_png );
  lv_img_set_src( ui_imgWiFiStatus2,  &ui_img_wifi_disconnected_png );
  ESP_LOGI( TAG, "gui_wifi_connecting" );
}

/**
 * @brief Callback function when ESP32 is connected to Router
 * @param data 
 */
static void gui_wifi_connected( uint8_t *data )
{
  // update the connect icon status to wifi connected with no internet
  lv_img_set_src( ui_imgWiFiStatus1,  &ui_img_wifi_png );
  lv_img_set_src( ui_imgWiFiStatus2,  &ui_img_wifi_png );
  ESP_LOGI( TAG, "gui_wifi_connected" );
}

/**
 * @brief Callback function when ESP32 is connected to router and also get time 
          from SNTP
 * @param data
 */
static void gui_wifi_internet_connected( uint8_t *data )
{
  // update the connect icon status to connected
  lv_img_set_src( ui_imgWiFiStatus1,  &ui_img_wifi_connected_png );
  lv_img_set_src( ui_imgWiFiStatus2,  &ui_img_wifi_connected_png );

  // this will be used to post another event to load sensor screen
  load_sensor_screen_timer = LOAD_SENSOR_SCREEN_TIMER;
  ESP_LOGI( TAG, "gui_wifi_internet_connected" );
  ESP_LOGW( TAG, "Time Starts to Load Next Screen ");
}

/**
 * @brief Callback function when WiFi is disconnected (Disconnect from Router & Influx Server)
 * @param data
 */
static void gui_wifi_disconnected( uint8_t *data )
{
  // update the connect icon status to disconnected
  lv_img_set_src( ui_imgWiFiStatus1,  &ui_img_wifi_disconnected_png );
  lv_img_set_src( ui_imgWiFiStatus2,  &ui_img_wifi_disconnected_png );
  ESP_LOGI( TAG, "gui_wifi_disconnected" );
}

/**
 * @brief Load the Sensor screen where temperature and humidity will be displayed
 * @param data 
 */
static void gui_load_sensor_screen( uint8_t *data )
{
  ESP_LOGI( TAG, "gui_load_sensor_screen" );
  // ui_SensorScreen_screen_init();
  if( ui_SensorScreen == NULL )
  {
    ESP_LOGE( TAG, "Sensor Screen Is Null" );
  }
  else
  {
    ESP_LOGW( TAG, "Loading Sensor Screen" );
    lv_disp_load_scr(ui_SensorScreen );
  }
}

/**
 * @brief Update the Temperature and Humidity data on display
 * @param data pointer to sensor data
 */
static void gui_update_sensor_data( uint8_t *data )
{
  sensor_data_t *sensor_data;
  sensor_data = (sensor_data_t*)data;
  ESP_LOGI( TAG, "gui_update_sensor_data" );
  uint8_t temperature = sensor_data->temperature_current;
  uint8_t humidity = sensor_data->humidity_current;
  lv_label_set_text_fmt( ui_lblSensor1, "%d°C", temperature );
  lv_arc_set_value( ui_arcSensor1, temperature );
  lv_label_set_text_fmt( ui_lblSensor2, "%d%%", humidity );
  lv_arc_set_value( ui_arcSensor2, humidity );
}