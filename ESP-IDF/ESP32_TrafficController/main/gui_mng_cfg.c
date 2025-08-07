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
#define NUM_OF_SIDES                    (4u)


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
static void gui_load_panel_1( uint8_t *data );
static void gui_update_traffic_led_1( uint8_t *data );
static void gui_update_traffic_led_2( uint8_t *data );
static void gui_update_traffic_led_3( uint8_t *data );
static void gui_update_traffic_led_4( uint8_t *data );

// Private Variables
static const gui_mng_event_cb_t gui_mng_event_cb[] =
{
  { GUI_MNG_EV_WIFI_CONNECTING,       gui_wifi_connecting       },
  { GUI_MNG_EV_MQTT_CONNECTING,       gui_mqtt_connecting       },
  { GUI_MNG_EV_MQTT_CONNECTED,        gui_load_panel_1          },
  { GUI_MNG_EV_UPDATE_TRAFFIC_LED_1,  gui_update_traffic_led_1  },
  { GUI_MNG_EV_UPDATE_TRAFFIC_LED_2,  gui_update_traffic_led_2  },
  { GUI_MNG_EV_UPDATE_TRAFFIC_LED_3,  gui_update_traffic_led_3  },
  { GUI_MNG_EV_UPDATE_TRAFFIC_LED_4,  gui_update_traffic_led_4  },
};
static lv_obj_t * led_green[NUM_OF_SIDES];
static lv_obj_t * led_yellow[NUM_OF_SIDES];
static lv_obj_t * led_red[NUM_OF_SIDES];
static lv_obj_t * panel_table[NUM_OF_SIDES];

// Public Function Definitions

/**
 * @brief gui manager configurable initialization
 *        this is added because I wanted to directly copy gui_mng and make this
 *        file configurable
 * @param  none
 */
void gui_cfg_init( void )
{
  uint8_t idx;
  ui_init();

  // updating array pointer with panel information for easy drawing
  panel_table[0] = ui_Panel1;
  panel_table[1] = ui_Panel2;
  panel_table[2] = ui_Panel3;
  panel_table[3] = ui_Panel4;

  // there are some widgets that are still not available in square line studio
  // hence creating them manually
  for( idx=0; idx<NUM_OF_SIDES; idx++ )
  {
    led_green[idx]  = lv_led_create( panel_table[idx] );
    led_yellow[idx] = lv_led_create( panel_table[idx] );
    led_red[idx]    = lv_led_create( panel_table[idx] );

    lv_obj_align(led_green[idx],  LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(led_yellow[idx], LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(led_red[idx],    LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_width(led_green[idx], 50);
    lv_obj_set_height(led_green[idx], 50);

    lv_obj_set_width(led_yellow[idx], 50);
    lv_obj_set_height(led_yellow[idx], 50);

    lv_obj_set_width(led_red[idx], 50);
    lv_obj_set_height(led_red[idx], 50);

    // adjusting green led offset from center
    lv_obj_set_x(led_green[idx], 0);
    lv_obj_set_y(led_green[idx], -70);
    // updating green color
    lv_led_set_color(led_green[idx], lv_palette_main(LV_PALETTE_GREEN));
    // updating yellow color
    lv_led_set_color(led_yellow[idx], lv_palette_main(LV_PALETTE_YELLOW));
    // adjusting red led offset from center
    lv_obj_set_x(led_red[idx], 0);
    lv_obj_set_y(led_red[idx], 70);
    // updating red color
    lv_led_set_color(led_red[idx], lv_palette_main(LV_PALETTE_RED));
    // turn off all leds
    lv_led_off(led_green[idx]);
    lv_led_off(led_yellow[idx]);
    lv_led_off(led_red[idx]);
    // test code
    // lv_led_on(led_green[idx]);
    // lv_led_on(led_yellow[idx]);
    // lv_led_on(led_red[idx]);
    // turn on Red Led only
    lv_led_on(led_red[idx]);
  }
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
}


/**
 * @brief Callback function to update the traffic LEDs of side-1
 * @param data pointer to traffic LEDs data
 */
static void gui_update_traffic_led_1( uint8_t *data )
{
  uint8_t traffic_led_status = *data;
  switch ( traffic_led_status )
  {
    case TRAFFIC_LED_GREEN:
      printf( "Green \r\n ");
      lv_led_on(led_green[0]);
      lv_led_off(led_yellow[0]);
      lv_led_off(led_red[0]);
      break;
    case TRAFFIC_LED_YELLOW:
      printf( "Yellow \r\n ");
      lv_led_off(led_green[0]);
      lv_led_on(led_yellow[0]);
      lv_led_off(led_red[0]);
      break;
    case TRAFFIC_LED_RED:
      printf( "Red \r\n ");
      lv_led_off(led_green[0]);
      lv_led_off(led_yellow[0]);
      lv_led_on(led_red[0]);
      break;
    default:
      printf( "Invalid: %d \r\n ", traffic_led_status);
      lv_led_off(led_green[0]);
      lv_led_off(led_yellow[0]);
      lv_led_off(led_red[0]);
      break;
  };
}

/**
 * @brief Callback function to update the traffic LEDs of side-2
 * @param data pointer to traffic LEDs data
 */
static void gui_update_traffic_led_2( uint8_t *data )
{

}

/**
 * @brief Callback function to update the traffic LEDs of side-3
 * @param data pointer to traffic LEDs data
 */
static void gui_update_traffic_led_3( uint8_t *data )
{

}

/**
 * @brief Callback function to update the traffic LEDs of side-4
 * @param data pointer to traffic LEDs data
 */
static void gui_update_traffic_led_4( uint8_t *data )
{

}
