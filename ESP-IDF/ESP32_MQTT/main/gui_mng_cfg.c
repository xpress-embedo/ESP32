/*
 * gui_mng_cfg.c
 *
 *  Created on: Feb 24, 2024
 *      Author: xpress_embedo
 */
#include "esp_log.h"
#include "ui.h"
#include "main.h"
#include "gui_mng.h"
#include "gui_mng_cfg.h"

// Private Macros
#define NUM_ELEMENTS(x)                 (sizeof(x)/sizeof(x[0]))


// function template for callback function
typedef void (*gui_mng_callback)(uint8_t * data);

typedef struct _gui_mng_event_cb_t
{
  gui_mng_event_t   event;
  gui_mng_callback  callback;
} gui_mng_event_cb_t;

// Private Functions
static void gui_wifi_connecting( uint8_t *data );
static void gui_mqtt_connecting( uint8_t *data );
static void gui_load_dashboard( uint8_t *data );
static void gui_update_sensor_data( uint8_t *data );

// Parivate Variables
static const char *TAG = "GUI_CFG";
static const gui_mng_event_cb_t gui_mng_event_cb[] =
{
  { GUI_MNG_EV_WIFI_CONNECTING,     gui_wifi_connecting     },
  { GUI_MNG_EV_MQTT_CONNECTING,     gui_mqtt_connecting     },
  { GUI_MNG_EV_MQTT_CONNECTED,      gui_load_dashboard      },
  { GUI_MNG_EV_TEMP_HUMID,          gui_update_sensor_data  },
};

// Public Function Definitions

/**
 * @brief gui manager configurable initialization
 *        this is added because I wanted to directly copy gui_mng and make this
 *        file configurable
 * @param  none
 */
void gui_cfg_init( void )
{
  ui_init();
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
 * @brief Callback function when ESP32 is connecting to WiFi router
 * @param data 
 */
static void gui_wifi_connecting( uint8_t *data )
{
  lv_label_set_text(ui_lblConnecting, "Connecting with Router............");
}

/**
 * @brief Callback function when ESP32 is connecting to MQTT Broker
 * @param data 
 */
static void gui_mqtt_connecting( uint8_t *data )
{
  lv_label_set_text(ui_lblConnecting, "Connecting with MQTT Broker........");
}

/**
 * @brief Load the Dashboard screem where all elements/widgets are available for
 *        visualization of sensor data and MQTT example
 * @param data 
 */
static void gui_load_dashboard( uint8_t *data )
{
  lv_disp_load_scr(ui_Dashboard);
}

/**
 * @brief Update the Temperature and Humidity data on display
 * @param data pointer to sensor data
 */
static void gui_update_sensor_data( uint8_t *data )
{
  sensor_data_t *sensor_data;
  sensor_data = (sensor_data_t*)data;
  lv_label_set_text_fmt(ui_lblTemperatureValue, "%d °C", sensor_data->temperature );
  lv_label_set_text_fmt(ui_lblHumidityValue, "%d %%", sensor_data->humidity );
}
