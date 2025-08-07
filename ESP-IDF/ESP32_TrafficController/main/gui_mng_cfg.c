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
static void gui_load_dashboard( uint8_t *data );
static void gui_update_sensor_data( uint8_t *data );
static void gui_update_switch_led( uint8_t *data );
static void gui_update_rgb_led( uint8_t * data );

// Private Variables
static const gui_mng_event_cb_t gui_mng_event_cb[] =
{
  { GUI_MNG_EV_WIFI_CONNECTING,     gui_wifi_connecting     },
  { GUI_MNG_EV_MQTT_CONNECTING,     gui_mqtt_connecting     },
  { GUI_MNG_EV_MQTT_CONNECTED,      gui_load_dashboard      },
  { GUI_MNG_EV_TEMP_HUMID,          gui_update_sensor_data  },
  { GUI_MNG_EV_SWITCH_LED,          gui_update_switch_led   },
  { GUI_MNG_EV_RGB_LED,             gui_update_rgb_led      },
};
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
 * @brief Load the Dashboard screem where all elements/widgets are available for
 *        visualization of sensor data and MQTT example
 * @param data 
 */
static void gui_load_dashboard( uint8_t *data )
{
  lv_disp_load_scr(ui_Dashboard);
  // there are some widgets that are still not available in square line studio
  // hence creating them manually
  // switch_led = lv_led_create( lv_scr_act() );
  switch_led = lv_led_create( ui_Dashboard );
  lv_obj_align(switch_led, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_width(switch_led, 100);
  lv_obj_set_height(switch_led, 100);
  // adjusting offset from center
  lv_obj_set_x(switch_led, -60);
  lv_obj_set_y(switch_led, 20);
  lv_led_set_color(switch_led, lv_palette_main(LV_PALETTE_RED));
  // turn off the led
  lv_led_off(switch_led);

  // switch control to control the switch led
  switch_led_ctrl = lv_switch_create(ui_Dashboard);
  lv_obj_set_width(switch_led_ctrl, 50);
  lv_obj_set_height(switch_led_ctrl, 25);
  lv_obj_set_x(switch_led_ctrl, -60);
  lv_obj_set_y(switch_led_ctrl, 90);
  lv_obj_set_align(switch_led_ctrl, LV_ALIGN_CENTER);
  lv_obj_set_style_bg_color(switch_led_ctrl, lv_color_hex(0x8B8583), LV_PART_KNOB | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(switch_led_ctrl, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
  lv_obj_add_event_cb(switch_led_ctrl, gui_switch_led_event, LV_EVENT_ALL, NULL);

  rgb_led = lv_obj_create( ui_Dashboard );
  lv_obj_align(rgb_led, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_width(rgb_led, 100);
  lv_obj_set_height(rgb_led, 100);
  lv_obj_set_style_radius(rgb_led, LV_RADIUS_CIRCLE, LV_PART_MAIN);
  // adjusting offset from center
  lv_obj_set_x(rgb_led, 60);
  lv_obj_set_y(rgb_led, 20);
  // Set style properties
  lv_obj_set_style_bg_color(rgb_led, lv_color_hex(0x1F1F1F), LV_STATE_DEFAULT);
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

/**
 * @brief Callback Function configured for Switch Led Control
 * @param e 
 */
static void gui_switch_led_event(lv_event_t * e)
{
  lv_event_code_t event_code = lv_event_get_code(e);
  lv_obj_t * target = lv_event_get_target(e);
  if(event_code == LV_EVENT_CLICKED) 
  {
    // toggle the led state
    if( lv_obj_has_state(target, LV_STATE_CHECKED) )
    {
      lv_led_on(switch_led);
      // app_publish_switch_led(true);
    }
    else
    {
      lv_led_off(switch_led);
      // app_publish_switch_led(false);
    }
    // lv_led_toggle(switch_led);
  }
}

/**
 * @brief Update the Switch Led state on display, and also publish event so that
 *        all other applications or devices synchronize with each other
 * @param data pointer to switch led state data
 */
static void gui_update_switch_led( uint8_t *data )
{
  // here we need to update the LED on display and also publish so that all
  // controls are also updated
  if( *data )
  {
    lv_led_on(switch_led);
    // to get the state use the function lv_obj_has_state(switch, LV_STATE_CHECKED)
    // and to se the state use the function lv_obj_add/clear_state(switch, LV_STATE_CHECKED)
    lv_obj_add_state(switch_led_ctrl, LV_STATE_CHECKED);
  }
  else
  {
    lv_led_off(switch_led);
    // clear the state
    lv_obj_clear_state(switch_led_ctrl, LV_STATE_CHECKED);
  }
}

/**
 * @brief Callback function to update the RGB Color of the LED based on rgb value
 * @param data pointer to rgb value data
 */
static void gui_update_rgb_led( uint8_t * data )
{
  int32_t *p_rgb = (int32_t*)(data);
  uint32_t rgb_color = (uint32_t)(*p_rgb & 0xFFFFFF);
  printf("RGB Color: %lu\r\n", rgb_color);
  lv_obj_set_style_bg_color(rgb_led, lv_color_hex(rgb_color), LV_STATE_DEFAULT);
}
