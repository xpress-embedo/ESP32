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
static void gui_switch_led_event(lv_event_t * e);
static void gui_wifi_connecting( uint8_t *data );
static void gui_mqtt_connecting( uint8_t *data );
static void gui_load_panel_1( uint8_t *data );
static void gui_update_sensor_data( uint8_t *data );
static void gui_update_switch_led( uint8_t *data );
static void gui_update_rgb_led( uint8_t * data );

// Private Variables
static const gui_mng_event_cb_t gui_mng_event_cb[] =
{
  { GUI_MNG_EV_WIFI_CONNECTING,     gui_wifi_connecting     },
  { GUI_MNG_EV_MQTT_CONNECTING,     gui_mqtt_connecting     },
  { GUI_MNG_EV_MQTT_CONNECTED,      gui_load_panel_1      },
  { GUI_MNG_EV_TEMP_HUMID,          gui_update_sensor_data  },
  { GUI_MNG_EV_SWITCH_LED,          gui_update_switch_led   },
  { GUI_MNG_EV_RGB_LED,             gui_update_rgb_led      },
};
static lv_obj_t * led_green;
static lv_obj_t * led_yellow;
static lv_obj_t * led_red;

static lv_obj_t * switch_led;
static lv_obj_t * switch_led_ctrl;
static lv_obj_t * rgb_led;

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
 * @brief Load the Panel-1 screen where all elements/widgets are available for
 *        visualization of Traffic Light
 * @param data 
 */
static void gui_load_panel_1( uint8_t *data )
{
  lv_disp_load_scr(ui_Panel1);
  // there are some widgets that are still not available in square line studio
  // hence creating them manually
  led_green   = lv_led_create( ui_Panel1 );
  led_yellow  = lv_led_create( ui_Panel1 );
  led_red     = lv_led_create( ui_Panel1 );

  lv_obj_align(led_green,   LV_ALIGN_CENTER, 0, 0);
  lv_obj_align(led_yellow,  LV_ALIGN_CENTER, 0, 0);
  lv_obj_align(led_red,     LV_ALIGN_CENTER, 0, 0);

  lv_obj_set_width(led_green, 50);
  lv_obj_set_height(led_green, 50);

  lv_obj_set_width(led_yellow, 50);
  lv_obj_set_height(led_yellow, 50);

  lv_obj_set_width(led_red, 50);
  lv_obj_set_height(led_red, 50);

  // adjusting green led offset from center
  lv_obj_set_x(led_green, 0);
  lv_obj_set_y(led_green, -70);
  // updating green color
  lv_led_set_color(led_green, lv_palette_main(LV_PALETTE_GREEN));

  // updating yellow color
  lv_led_set_color(led_yellow, lv_palette_main(LV_PALETTE_YELLOW));

  // adjusting red led offset from center
  lv_obj_set_x(led_red, 0);
  lv_obj_set_y(led_red, 70);
  // updating red color
  lv_led_set_color(led_red, lv_palette_main(LV_PALETTE_RED));

  // turn off all leds
  lv_led_off(led_green);
  lv_led_off(led_yellow);
  lv_led_off(led_red);

  // test code
  lv_led_on(led_green);
  lv_led_on(led_yellow);
  lv_led_on(led_red);
}

/**
 * @brief Update the Temperature and Humidity data on display
 * @param data pointer to sensor data
 */
static void gui_update_sensor_data( uint8_t *data )
{
}

/**
 * @brief Callback Function configured for Switch Led Control
 * @param e 
 */
static void gui_switch_led_event(lv_event_t * e)
{
}

/**
 * @brief Update the Switch Led state on display, and also publish event so that
 *        all other applications or devices synchronize with each other
 * @param data pointer to switch led state data
 */
static void gui_update_switch_led( uint8_t *data )
{
}

/**
 * @brief Callback function to update the RGB Color of the LED based on rgb value
 * @param data pointer to rgb value data
 */
static void gui_update_rgb_led( uint8_t * data )
{

}
