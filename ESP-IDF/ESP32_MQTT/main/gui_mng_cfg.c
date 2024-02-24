/*
 * gui_mng_cfg.c
 *
 *  Created on: Feb 24, 2024
 *      Author: xpress_embedo
 */
#include "esp_log.h"
#include "ui.h"
#include "gui_mng.h"
#include "gui_mng_cfg.h"

#define NUM_ELEMENTS(x)                 (sizeof(x)/sizeof(x[0]))

// function template for callback function
typedef void (*gui_mng_callback)(uint8_t * data);

typedef struct _gui_mng_event_cb_t
{
  gui_mng_event_t   event;
  gui_mng_callback  callback;
} gui_mng_event_cb_t;

static void gui_wifi_connecting( uint8_t *data );
static void gui_mqtt_connecting( uint8_t *data );

// static const char *TAG = "GUI_CFG";
static const gui_mng_event_cb_t gui_mng_event_cb[] =
{
  { GUI_MNG_EV_WIFI_CONNECTING,    gui_wifi_connecting    },
  { GUI_MNG_EV_MQTT_CONNECTING,    gui_mqtt_connecting    },
};

void gui_cfg_init( void )
{
  ui_init();
}

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

static void gui_wifi_connecting( uint8_t *data )
{
  lv_label_set_text(ui_lblConnecting, "Connecting with Router....");
}

static void gui_mqtt_connecting( uint8_t *data )
{
  lv_label_set_text(ui_lblConnecting, "Connecting with MQTT Server..");
}
