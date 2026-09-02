/*
 * gui_mng_cfg.c
 *
 *  Created on: Jun 15, 2024
 *      Author: xpress_embedo
 */

#include "esp_log.h"

#include <stdio.h>

#include "lvgl.h"
#include "gui_mng_cfg.h"
#include "ui.h"

// Private Macros
#define NUM_ELEMENTS(x)                 (sizeof(x)/sizeof(x[0]))

// Each GUI event can be connected to one handler function with this type.
typedef void (*gui_mng_callback)(const gui_mng_event_data_t *data);

// One row in the event-to-callback table.
typedef struct _gui_mng_event_cb_t
{
  gui_mng_event_t   event;
  gui_mng_callback  callback;
} gui_mng_event_cb_t;

// Private Function Prototypes
static void gui_startup( const gui_mng_event_data_t *data );
static void gui_IR_command_received( const gui_mng_event_data_t *data );

// Private Variables
static const char *TAG = "GUI_CFG";
static lv_obj_t *s_ir_signal_label = NULL;

static const gui_mng_event_cb_t gui_mng_event_cb[] =
{
  { GUI_MNG_EV_STARTUP,                 gui_startup                   },
  { GUI_MNG_EV_IR_COMMAND,              gui_IR_command_received       },
};

// Public Function Definitions
/**
 * @brief GUI Configurable Initialization Function
 * @param  None
 */
void gui_cfg_init( void )
{
  ESP_LOGI( TAG, "UI Init. Starts" );
  ui_init( NULL );
  ESP_LOGI( TAG, "UI Init. Ends" );

  // Create and load the LVGL Pro generated main screen
  lv_obj_t *scr = screen_main_create();
  lv_screen_load( scr );

  // Step 1: Find your generated label by name and store pointer for reuse.
  // This avoids editing generated C file internals.
  s_ir_signal_label = lv_obj_find_by_name( scr, "lv_label_IR_signal" );
  if( s_ir_signal_label == NULL )
  {
    ESP_LOGW( TAG, "Label lv_label_IR_signal not found" );
  }
}

/**
 * @brief Process the events posted to GUI manager module
 *        This function calls the dedicated function based on the event posted
 *        to GUI manager queue.
 *
 * Simple idea:
 * - an event number comes in
 * - we search the table
 * - if we find matching event, we call its function
 *
 * @param event Event name.
 * @param data Pointer to copied typed event data.
 */
void gui_cfg_mng_process( gui_mng_event_t event, const gui_mng_event_data_t *data )
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

      break;
    }
  }
}

/**
 * @brief this is a custom refresh function called periodically by GUI manager
 *        in this function we can write our code which can be called periodically
 */
void gui_cfg_refresh( void )
{
}

// Private Function Definitions
// These variables are kept static so only this file can access this screen state.
static lv_obj_t *s_uptime_label = NULL;
static lv_obj_t *s_touch_label = NULL;
static uint32_t s_uptime_seconds = 0;
static uint32_t s_touch_count = 0;

/**
 * @brief Refresh touch counter label immediately.
 *
 * This helper is called from both event and timer paths so there is one
 * place that formats the touch counter text.
 */
static void starter_ui_update_touch_label( void )
{
  if( s_touch_label != NULL )
  {
    char touch_text[48];
    snprintf( touch_text, sizeof(touch_text), "Touch taps: %lu", (unsigned long)s_touch_count );
    lv_label_set_text( s_touch_label, touch_text );
  }
}

/**
 * @brief LVGL event callback for touch press.
 *
 * We count on LV_EVENT_PRESSED so feedback feels immediate when finger touches
 * the panel, rather than waiting for release.
 *
 * @param e LVGL event object.
 */
static void starter_ui_touch_event_cb( lv_event_t *e )
{
  if( lv_event_get_code(e) == LV_EVENT_PRESSED )
  {
    s_touch_count++;
    starter_ui_update_touch_label();
  }
}

/**
 * @brief LVGL 1-second timer callback.
 *
 * Updates uptime text every second. Touch counter is also refreshed from here
 * as a safety refresh, while primary touch updates happen in real-time from
 * the touch event callback.
 *
 * @param timer LVGL timer handle (unused).
 */
static void starter_ui_timer_cb( lv_timer_t *timer )
{
  (void) timer;
  s_uptime_seconds++;

  if( s_uptime_label != NULL )
  {
    char uptime_text[48];
    snprintf( uptime_text, sizeof(uptime_text), "Uptime: %lu s", (unsigned long)s_uptime_seconds );
    lv_label_set_text( s_uptime_label, uptime_text );
  }

  starter_ui_update_touch_label();
}

/**
 * @brief Build a simple LVGL starter screen.
 *
 * This is called when GUI_MNG_EV_STARTUP is processed.
 *
 * The screen has:
 * - title/subtitle
 * - uptime counter
 * - touch counter
 * - hint text for validation
 */
static void gui_startup( const gui_mng_event_data_t *data )
{
  (void) data;

  // Get current active screen (root container in LVGL).
  lv_obj_t *screen = lv_screen_active();
  lv_obj_set_style_bg_color( screen, lv_color_hex(0xEAF4FF), LV_PART_MAIN );
  lv_obj_set_style_bg_opa( screen, LV_OPA_COVER, LV_PART_MAIN );
  lv_obj_add_event_cb( screen, starter_ui_touch_event_cb, LV_EVENT_PRESSED, NULL );

  // Main card widget used as a simple demo panel.
  lv_obj_t *card = lv_obj_create( screen );
  lv_obj_set_size( card, 560, 300 );
  lv_obj_center( card );
  lv_obj_set_style_radius( card, 18, LV_PART_MAIN );
  lv_obj_set_style_bg_color( card, lv_color_hex(0xFFFFFF), LV_PART_MAIN );
  lv_obj_set_style_border_width( card, 2, LV_PART_MAIN );
  lv_obj_set_style_border_color( card, lv_color_hex(0x8AB4F8), LV_PART_MAIN );
  lv_obj_set_style_shadow_width( card, 18, LV_PART_MAIN );
  lv_obj_set_style_shadow_opa( card, LV_OPA_20, LV_PART_MAIN );
  lv_obj_set_style_shadow_color( card, lv_color_hex(0x5B7DB0), LV_PART_MAIN );

  // Create title label.
  lv_obj_t *title = lv_label_create( card );
  lv_label_set_text( title, "LVGL 9.5 Validation" );
  lv_obj_set_style_text_font( title, &lv_font_montserrat_14, 0 );
  lv_obj_set_style_text_color( title, lv_color_hex(0x1E3556), 0 );
  lv_obj_align( title, LV_ALIGN_TOP_MID, 0, 20 );

  // Create subtitle to confirm board/display details.
  lv_obj_t *subtitle = lv_label_create( card );
  lv_label_set_text( subtitle, "Sunton ESP32-8048S043 is rendering and updating." );
  lv_obj_set_style_text_font( subtitle, &lv_font_montserrat_14, 0 );
  lv_obj_set_style_text_color( subtitle, lv_color_hex(0x3D587E), 0 );
  lv_obj_align_to( subtitle, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 14 );

  // Dynamic label: uptime seconds.
  s_uptime_label = lv_label_create( card );
  lv_label_set_text( s_uptime_label, "Uptime: 0 s" );
  lv_obj_set_style_text_font( s_uptime_label, &lv_font_montserrat_14, 0 );
  lv_obj_align( s_uptime_label, LV_ALIGN_CENTER, 0, 18 );

  // Dynamic label: number of touches.
  s_touch_label = lv_label_create( card );
  lv_label_set_text( s_touch_label, "Touch taps: 0" );
  lv_obj_set_style_text_font( s_touch_label, &lv_font_montserrat_14, 0 );
  lv_obj_align_to( s_touch_label, s_uptime_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 14 );

  // Ensure label formatting comes from one helper function.
  starter_ui_update_touch_label();

  // User hint label.
  lv_obj_t *hint = lv_label_create( card );
  lv_label_set_text( hint, "Tap anywhere on screen to validate touch." );
  lv_obj_set_style_text_font( hint, &lv_font_montserrat_14, 0 );
  lv_obj_set_style_text_color( hint, lv_color_hex(0x5A6F90), 0 );
  lv_obj_align( hint, LV_ALIGN_BOTTOM_MID, 0, -18 );

  // Periodic timer for updating uptime/touch labels.
  lv_timer_create( starter_ui_timer_cb, 1000, NULL );
}

static void gui_IR_command_received( const gui_mng_event_data_t *data )
{
  if( data == NULL )
  {
    return;
  }

  ESP_LOGI( TAG, "IR Command Received: %s", data->infrared.command_msg );

  // Step 2: Update TFT label text on every IR command event.
  // This callback runs in GUI task context, so LVGL call is safe here.
  if( s_ir_signal_label != NULL )
  {
    lv_label_set_text( s_ir_signal_label, data->infrared.command_msg );
  }
  else
  {
    ESP_LOGW( TAG, "IR label pointer is NULL, cannot update text" );
  }
}

