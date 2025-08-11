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
#include "mqtt_app.h"

// Private Macros
#define NUM_ELEMENTS(x)                 (sizeof(x)/sizeof(x[0]))
#define NUM_OF_SIDES                    (4u)
#define LOAD_PANEL_TIME_COUNTER         ( 1000/ GUI_MNG_REFRESH_TIME )


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
static void gui_mqtt_connected( uint8_t *data );
static void gui_serial_connected( uint8_t *data );
static void gui_wifi_mqtt_disconnected( uint8_t *data );
static void gui_load_traffic_panel( uint8_t *data );
static void gui_update_traffic_led_1( uint8_t *data );
static void gui_update_traffic_led_2( uint8_t *data );
static void gui_update_traffic_led_3( uint8_t *data );
static void gui_update_traffic_led_4( uint8_t *data );
static void gui_update_traffic_time_1( uint8_t *data );
static void gui_update_traffic_time_2( uint8_t *data );
static void gui_update_traffic_time_3( uint8_t *data );
static void gui_update_traffic_time_4( uint8_t *data );
static void gui_update_traffic_lights_v2( uint8_t *data );

// Private Variables
static const gui_mng_event_cb_t gui_mng_event_cb[] =
{
  { GUI_MNG_EV_WIFI_CONNECTING,       gui_wifi_connecting           },
  { GUI_MNG_EV_MQTT_CONNECTING,       gui_mqtt_connecting           },
  { GUI_MNG_EV_MQTT_CONNECTED,        gui_mqtt_connected            },
  { GUI_MNG_EV_WIFI_DISCONNECTED,     gui_wifi_mqtt_disconnected    },
  { GUI_MNG_EV_SERIAL_CONNECTED,      gui_serial_connected          },
  { GUI_MNG_EV_LOAD_TRAFFIC_PANEL,    gui_load_traffic_panel        },
  { GUI_MNG_EV_TRAFFIC_LED_1,         gui_update_traffic_led_1      },
  { GUI_MNG_EV_TRAFFIC_LED_2,         gui_update_traffic_led_2      },
  { GUI_MNG_EV_TRAFFIC_LED_3,         gui_update_traffic_led_3      },
  { GUI_MNG_EV_TRAFFIC_LED_4,         gui_update_traffic_led_4      },
  { GUI_MNG_EV_TRAFFIC_TIME_1,        gui_update_traffic_time_1     },
  { GUI_MNG_EV_TRAFFIC_TIME_2,        gui_update_traffic_time_2     },
  { GUI_MNG_EV_TRAFFIC_TIME_3,        gui_update_traffic_time_3     },
  { GUI_MNG_EV_TRAFFIC_TIME_4,        gui_update_traffic_time_4     },
  { GUI_MNG_EV_TRAFFIC_CTRL_V2,       gui_update_traffic_lights_v2  },
};
static lv_obj_t * led_green[NUM_OF_SIDES];
static lv_obj_t * led_yellow[NUM_OF_SIDES];
static lv_obj_t * led_red[NUM_OF_SIDES];
static lv_obj_t * container_led_green[NUM_OF_SIDES];
static lv_obj_t * container_led_yellow[NUM_OF_SIDES];
static lv_obj_t * container_led_red[NUM_OF_SIDES];
static lv_obj_t * green_time[NUM_OF_SIDES];
static lv_obj_t * yellow_time[NUM_OF_SIDES];
static lv_obj_t * red_time[NUM_OF_SIDES];
static lv_obj_t * container_green_time[NUM_OF_SIDES];
static lv_obj_t * container_yellow_time[NUM_OF_SIDES];
static lv_obj_t * container_red_time[NUM_OF_SIDES];
static lv_obj_t * panel_table[NUM_OF_SIDES];

static uint32_t load_panel_timer = 0;

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

  green_time[0]  = ui_lblGreenTime1;
  green_time[1]  = ui_lblGreenTime2;
  green_time[2]  = ui_lblGreenTime3;
  green_time[3]  = ui_lblGreenTime4;
  
  yellow_time[0] = ui_lblYellowTime1;
  yellow_time[1] = ui_lblYellowTime2;
  yellow_time[2] = ui_lblYellowTime3;
  yellow_time[3] = ui_lblYellowTime4;

  red_time[0]    = ui_lblRedTime1;
  red_time[1]    = ui_lblRedTime2;
  red_time[2]    = ui_lblRedTime3;
  red_time[3]    = ui_lblRedTime4;

  container_led_green[0]  = ui_containerGreen1;
  container_led_green[1]  = ui_containerGreen2;
  container_led_green[2]  = ui_containerGreen3;
  container_led_green[3]  = ui_containerGreen4;
  container_led_yellow[0] = ui_containerYellow1;
  container_led_yellow[1] = ui_containerYellow2;
  container_led_yellow[2] = ui_containerYellow3;
  container_led_yellow[3] = ui_containerYellow4;
  container_led_red[0]    = ui_containerRed1;
  container_led_red[1]    = ui_containerRed2;
  container_led_red[2]    = ui_containerRed3;
  container_led_red[3]    = ui_containerRed4;

  container_green_time[0]  = ui_lblContainerGreenTime1;
  container_green_time[1]  = ui_lblContainerGreenTime2;
  container_green_time[2]  = ui_lblContainerGreenTime3;
  container_green_time[3]  = ui_lblContainerGreenTime4;
  container_yellow_time[0] = ui_lblContainerYellowTime1;
  container_yellow_time[1] = ui_lblContainerYellowTime2;
  container_yellow_time[2] = ui_lblContainerYellowTime3;
  container_yellow_time[3] = ui_lblContainerYellowTime4;
  container_red_time[0]    = ui_lblContainerRedTime1;
  container_red_time[1]    = ui_lblContainerRedTime2;
  container_red_time[2]    = ui_lblContainerRedTime3;
  container_red_time[3]    = ui_lblContainerRedTime4;

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
    lv_obj_set_x(led_green[idx], -60);
    lv_obj_set_y(led_green[idx], -40);
    // updating green color
    lv_led_set_color(led_green[idx], lv_palette_main(LV_PALETTE_GREEN));
    // lv_led_set_color(led_green[idx], lv_color_hex(0x008000));

    // adjusting yellow led offset from center
    lv_obj_set_x(led_yellow[idx], -60);
    lv_obj_set_y(led_yellow[idx], 20);
    // updating yellow color
    lv_led_set_color(led_yellow[idx], lv_palette_main(LV_PALETTE_YELLOW));
    // lv_led_set_color(led_green[idx], lv_color_hex(0xC8C800));

    // adjusting red led offset from center
    lv_obj_set_x(led_red[idx], -60);
    lv_obj_set_y(led_red[idx], 80);
    // updating red color
    lv_led_set_color(led_red[idx], lv_palette_main(LV_PALETTE_RED));
    // lv_led_set_color(led_green[idx], lv_color_hex(0x800000));

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
 * @brief this is a custom refresh function called periodically by GUI manager
 *        in this function we can write our code which can be called periodically
 */
void gui_cfg_refresh( void )
{
  if( (load_panel_timer > 0) && (--load_panel_timer == 0) )
  {
    gui_send_event( GUI_MNG_EV_LOAD_TRAFFIC_PANEL, NULL );
  }
}

/**
 * @brief Callback function when ESP32 is connecting to WiFi router
 * @param data 
 */
static void gui_wifi_connecting( uint8_t *data )
{
  // update the connect icon status to disconnected
  lv_img_set_src( ui_imgConnectStatus,  &ui_img_1402433841 );
  lv_img_set_src( ui_imgConnectStatus1, &ui_img_1402433841 );
  lv_img_set_src( ui_imgConnectStatus2, &ui_img_1402433841 );
  lv_img_set_src( ui_imgConnectStatus3, &ui_img_1402433841 );
  lv_img_set_src( ui_imgConnectStatus4, &ui_img_1402433841 );
  lv_img_set_src( ui_imgConnectStatusAllTraffic, &ui_img_1402433841 );
}

/**
 * @brief Callback function when ESP32 is connecting to MQTT Broker
 * @param data 
 */
static void gui_mqtt_connecting( uint8_t *data )
{
  // update the connect icon status to wifi connected with no internet
  lv_img_set_src( ui_imgConnectStatus,  &ui_img_1688301267 );
  lv_img_set_src( ui_imgConnectStatus1, &ui_img_1688301267 );
  lv_img_set_src( ui_imgConnectStatus2, &ui_img_1688301267 );
  lv_img_set_src( ui_imgConnectStatus3, &ui_img_1688301267 );
  lv_img_set_src( ui_imgConnectStatus4, &ui_img_1688301267 );
  lv_img_set_src( ui_imgConnectStatusAllTraffic, &ui_img_1688301267 );
}

/**
 * @brief Callback function when ESP32 is connecting to MQTT Broker
 * @param data
 */
static void gui_mqtt_connected( uint8_t *data )
{
  // update the connect icon status to connected
  lv_img_set_src( ui_imgConnectStatus,  &ui_img_338993590 );
  lv_img_set_src( ui_imgConnectStatus1, &ui_img_338993590 );
  lv_img_set_src( ui_imgConnectStatus2, &ui_img_338993590 );
  lv_img_set_src( ui_imgConnectStatus3, &ui_img_338993590 );
  lv_img_set_src( ui_imgConnectStatus4, &ui_img_338993590 );
  lv_img_set_src( ui_imgConnectStatusAllTraffic, &ui_img_338993590 );

  // this will be used to post another event to load panel-1
  load_panel_timer = LOAD_PANEL_TIME_COUNTER;
}

/**
 * @brief Callback function when WiFi is disconnected (Disconnect from Router & MQTT Server)
 * @param data
 */
static void gui_wifi_mqtt_disconnected( uint8_t *data )
{
  // update the connect icon status to disconnected
  lv_img_set_src( ui_imgConnectStatus,  &ui_img_1402433841 );
  lv_img_set_src( ui_imgConnectStatus1, &ui_img_1402433841 );
  lv_img_set_src( ui_imgConnectStatus2, &ui_img_1402433841 );
  lv_img_set_src( ui_imgConnectStatus3, &ui_img_1402433841 );
  lv_img_set_src( ui_imgConnectStatus4, &ui_img_1402433841 );
  lv_img_set_src( ui_imgConnectStatusAllTraffic, &ui_img_1402433841 );
}

/**
 * @brief Callback function when ESP32 is using Serial Communication
 * @param data
 */
static void gui_serial_connected( uint8_t *data )
{
  // this will be used to post another event to load panel-1
  load_panel_timer = LOAD_PANEL_TIME_COUNTER;
}

/**
 * @brief Load the Panel-1 screen where all elements/widgets are available for
 *        visualization of Traffic Light
 * @param data 
 */
static void gui_load_traffic_panel( uint8_t *data )
{
  lv_disp_load_scr(ui_PanelAllTraffic);
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
      lv_led_on(led_green[0]);
      lv_led_off(led_yellow[0]);
      lv_led_off(led_red[0]);
      break;
    case TRAFFIC_LED_YELLOW:
      lv_led_off(led_green[0]);
      lv_led_on(led_yellow[0]);
      lv_led_off(led_red[0]);
      break;
    case TRAFFIC_LED_RED:
      lv_led_off(led_green[0]);
      lv_led_off(led_yellow[0]);
      lv_led_on(led_red[0]);
      break;
    default:
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
  uint8_t traffic_led_status = *data;
  switch ( traffic_led_status )
  {
    case TRAFFIC_LED_GREEN:
      lv_led_on(led_green[1]);
      lv_led_off(led_yellow[1]);
      lv_led_off(led_red[1]);
      break;
    case TRAFFIC_LED_YELLOW:
      lv_led_off(led_green[1]);
      lv_led_on(led_yellow[1]);
      lv_led_off(led_red[1]);
      break;
    case TRAFFIC_LED_RED:
      lv_led_off(led_green[1]);
      lv_led_off(led_yellow[1]);
      lv_led_on(led_red[1]);
      break;
    default:
      lv_led_off(led_green[1]);
      lv_led_off(led_yellow[1]);
      lv_led_off(led_red[1]);
      break;
  };
}

/**
 * @brief Callback function to update the traffic LEDs of side-3
 * @param data pointer to traffic LEDs data
 */
static void gui_update_traffic_led_3( uint8_t *data )
{
  uint8_t traffic_led_status = *data;
  switch ( traffic_led_status )
  {
    case TRAFFIC_LED_GREEN:
      lv_led_on(led_green[2]);
      lv_led_off(led_yellow[2]);
      lv_led_off(led_red[2]);
      break;
    case TRAFFIC_LED_YELLOW:
      lv_led_off(led_green[2]);
      lv_led_on(led_yellow[2]);
      lv_led_off(led_red[2]);
      break;
    case TRAFFIC_LED_RED:
      lv_led_off(led_green[2]);
      lv_led_off(led_yellow[2]);
      lv_led_on(led_red[2]);
      break;
    default:
      lv_led_off(led_green[2]);
      lv_led_off(led_yellow[2]);
      lv_led_off(led_red[2]);
      break;
  };
}

/**
 * @brief Callback function to update the traffic LEDs of side-4
 * @param data pointer to traffic LEDs data
 */
static void gui_update_traffic_led_4( uint8_t *data )
{
  uint8_t traffic_led_status = *data;
  switch ( traffic_led_status )
  {
    case TRAFFIC_LED_GREEN:
      lv_led_on(led_green[3]);
      lv_led_off(led_yellow[3]);
      lv_led_off(led_red[3]);
      break;
    case TRAFFIC_LED_YELLOW:
      lv_led_off(led_green[3]);
      lv_led_on(led_yellow[3]);
      lv_led_off(led_red[3]);
      break;
    case TRAFFIC_LED_RED:
      lv_led_off(led_green[3]);
      lv_led_off(led_yellow[3]);
      lv_led_on(led_red[3]);
      break;
    default:
      lv_led_off(led_green[3]);
      lv_led_off(led_yellow[3]);
      lv_led_off(led_red[3]);
      break;
  };
}

/**
 * @brief Callback function to update the traffic time of side-1
 * @param data pointer to traffic time data
 */
static void gui_update_traffic_time_1( uint8_t *data )
{
  lv_label_set_text_fmt( ui_lblGreenTime1, "%.2d", *data );
}

/**
 * @brief Callback function to update the traffic time of side-2
 * @param data pointer to traffic time data
 */
static void gui_update_traffic_time_2( uint8_t *data )
{
  lv_label_set_text_fmt( ui_lblGreenTime2, "%.2d", *data );
}

/**
 * @brief Callback function to update the traffic time of side-3
 * @param data pointer to traffic time data
 */
static void gui_update_traffic_time_3( uint8_t *data )
{
  lv_label_set_text_fmt( ui_lblGreenTime3, "%.2d", *data );
}

/**
 * @brief Callback function to update the traffic time of side-4
 * @param data pointer to traffic time data
 */
static void gui_update_traffic_time_4( uint8_t *data )
{
  lv_label_set_text_fmt( ui_lblGreenTime4, "%.2d", *data );
}

/**
 * @brief Callback function to update the whole traffic lights status as per v2
 * @param data pointer to traffic status version 2
 */
static void gui_update_traffic_lights_v2( uint8_t *data )
{
  uint8_t idx;
  traffic_light_t *traffic_light_data;
  traffic_light_data = (traffic_light_t*)data;

  for( idx=0; idx < TRAFFIC_LIGHT_SIDES; idx++ )
  {
    if( traffic_light_data[idx].green_time )
    {
      // Panel Traffic Lights
      lv_led_on( led_green[idx] );
      lv_led_off(led_yellow[idx] );
      lv_led_off( led_red[idx] );
      // Container Traffic Lights
      lv_obj_set_style_bg_color( container_led_green[idx], lv_color_hex(0x008000), LV_PART_MAIN | LV_STATE_DEFAULT);
      lv_obj_set_style_bg_color( container_led_yellow[idx], lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
      lv_obj_set_style_bg_color( container_led_red[idx], lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);

      // Panel Traffic Lights Time
      lv_label_set_text_fmt( green_time[idx],   "%.2d", traffic_light_data[idx].green_time );
      lv_label_set_text_fmt( yellow_time[idx],  "00" );
      lv_label_set_text_fmt( red_time[idx],     "00" );
      // Container Traffic Lights (here we update all LEDs)
      lv_label_set_text_fmt( container_green_time[idx],   "%.2d", traffic_light_data[idx].green_time );
      lv_label_set_text_fmt( container_yellow_time[idx],  "00" );
      lv_label_set_text_fmt( container_red_time[idx],     "00" );
    }
    else if( traffic_light_data[idx].yellow_time )
    {
      // Panel Traffic Lights
      lv_led_off( led_green[idx] );
      lv_led_on(led_yellow[idx] );
      lv_led_off( led_red[idx] );
      // Container Traffic Lights
      lv_obj_set_style_bg_color( container_led_green[idx], lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
      lv_obj_set_style_bg_color( container_led_yellow[idx], lv_color_hex(0x808000), LV_PART_MAIN | LV_STATE_DEFAULT);
      lv_obj_set_style_bg_color( container_led_red[idx], lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);

      // Panel Traffic Lights Time
      lv_label_set_text_fmt( yellow_time[idx],  "%.2d", traffic_light_data[idx].yellow_time );
      lv_label_set_text_fmt( green_time[idx],   "00" );
      lv_label_set_text_fmt( red_time[idx],     "00" );
      // Container Traffic Lights (here we update all LEDs)
      lv_label_set_text_fmt( container_yellow_time[idx],  "%.2d", traffic_light_data[idx].yellow_time );
      lv_label_set_text_fmt( container_green_time[idx],   "00" );
      lv_label_set_text_fmt( container_red_time[idx],     "00" );
    }
    else if( traffic_light_data[idx].red_time )
    {
      // Panel Traffic Lights
      lv_led_off( led_green[idx] );
      lv_led_off(led_yellow[idx] );
      lv_led_on( led_red[idx] );
      // Container Traffic Lights
      lv_obj_set_style_bg_color( container_led_green[idx], lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
      lv_obj_set_style_bg_color( container_led_yellow[idx], lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
      lv_obj_set_style_bg_color( container_led_red[idx], lv_color_hex(0x800000), LV_PART_MAIN | LV_STATE_DEFAULT);

      // Panel Traffic Lights
      lv_label_set_text_fmt( red_time[idx],   "%.2d", traffic_light_data[idx].red_time );
      lv_label_set_text_fmt( green_time[idx], "00" );
      lv_label_set_text_fmt( yellow_time[idx],"00" );
      // Container Traffic Lights (here we update all LEDs)
      lv_label_set_text_fmt( container_red_time[idx],   "%.2d", traffic_light_data[idx].red_time );
      lv_label_set_text_fmt( container_green_time[idx], "00" );
      lv_label_set_text_fmt( container_yellow_time[idx],"00" );
    }
    else
    {
      // some invalid case
    }
  }

}


