/*
 * gui_mng_cfg.c
 *
 *  Created on: Jun 15, 2024
 *      Author: xpress_embedo
 */

#include <stdio.h>
#include "ui.h"
#include "gui_mng_cfg.h"

/* Private Macros */
#define NUM_ELEMENTS(x)                 (sizeof(x)/sizeof(x[0]))

/* Each GUI event can be connected to one handler function with this type. */
typedef void (*gui_mng_callback)(const gui_mng_event_data_t *data);

/* One row in the event-to-callback table. */
typedef struct _gui_mng_event_cb_t
{
  gui_mng_event_t   event;
  gui_mng_callback  callback;
} gui_mng_event_cb_t;

/* Threshold Rule Structure */
typedef struct
{
  int32_t max_threshold;      /* Range Upper Limit */
  const char *text;           /* status text */
  uint32_t color;             /* status color */
} sensor_range_rule_t;

/* Private Function Prototypes */
static const sensor_range_rule_t * get_sensor_rule( int32_t value, const sensor_range_rule_t *rules, size_t count );
static void gui_startup_rgb_obsv_cb( lv_observer_t *observer, lv_subject_t *subject );
static void gui_startup( const gui_mng_event_data_t *data );
static void gui_load_sensor_screen( const gui_mng_event_data_t *data );
static void gui_sensor_data_update( const gui_mng_event_data_t *data );

/* Private Variables */
static const gui_mng_event_cb_t gui_mng_event_cb[] =
{
  { GUI_MNG_EV_STARTUP,                 gui_startup                     },
  { GUI_MNG_EV_LOAD_SENSOR_SCREEN,      gui_load_sensor_screen          },
  { GUI_MNG_EV_SENSOR_DATA_UPDATE,      gui_sensor_data_update          },
};

/* Temperature Rules (in °C) */
static const sensor_range_rule_t temp_rules[] = 
{
  { 10,  "Too Cold", 0x3498DB }, /* Blue */
  { 18,  "Cold",     0x5DADE2 }, /* Light Blue */
  { 26,  "Ideal",    0x2ECC71 }, /* Emerald Green */
  { 32,  "Warm",     0xF39C12 }, /* Orange */
  { 100, "Hot",      0xE74C3C }  /* Red */
};

/* Humidity Rules (in %RH) */
static const sensor_range_rule_t hum_rules[] =
{
  { 30,  "Too Dry",   0xE67E22 }, /* Amber */
  { 40,  "Dry",       0xF1C40F }, /* Yellow */
  { 60,  "Ideal",     0x00E5FF }, /* Glowing Cyan */
  { 70,  "Humid",     0x3498DB }, /* Blue */
  { 100, "Too Humid", 0x9B59B6 }  /* Purple */
};

/* Barometric Pressure Rules (in hPa) */
static const sensor_range_rule_t press_rules[] =
{
  { 1000, "Low (Rain)",   0xE74C3C }, /* Red (Rain/Stormy) */
  { 1020, "Stable",       0xBD10E0 }, /* Purple (Fair/Calm) */
  { 1200, "High (Clear)", 0x2ECC71 }  /* Green (Sunny) */
};

static lv_obj_t * active_sensor_screen = NULL;  /* Pointer to the currently active sensor screen */

/* Public Function Definitions */
/**
 * @brief GUI Configurable Initialization Function
 * @param  None
 */
void gui_cfg_init( void )
{
  ui_init( NULL );

  #if 0
  /* NOTE: Actual GUI Screen is loaded by GUI MNG Events */
  /* Create and load the LVGL Pro generated main screen */
  lv_obj_t *screen = main_screen_create();
  lv_screen_load( screen );
  #endif
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

/* Private Function Definitions */
/**
 * @brief Get the sensor rule based on the value and rules array
 * @param value The sensor value to evaluate
 * @param rules The array of sensor range rules
 * @param count The number of rules in the array
 * @return A pointer to the matching sensor range rule
 */
static const sensor_range_rule_t * get_sensor_rule( int32_t value, const sensor_range_rule_t *rules, size_t count )
{
  for ( size_t i = 0; i < count; i++ )
  {
    if ( value < rules[i].max_threshold )
    {
      return &rules[i];
    }
  }
  return &rules[count - 1]; /* Fallback to last rule if value exceeds all thresholds */
}


/**
 * @brief Update callback for the RGB observer
 * @param observer The observer object
 * @param subject The subject object
 */
static void gui_startup_rgb_obsv_cb( lv_observer_t *observer, lv_subject_t *subject )
{
  lv_obj_t *rectangle = lv_observer_get_target_obj( observer );

  /* Read the current value of each subject directly */
  int32_t r = lv_subject_get_int( &red_val );
  int32_t g = lv_subject_get_int( &green_val );
  int32_t b = lv_subject_get_int( &blue_val );

  /* Combine the three channel values and update the rectangle background */
  lv_obj_set_style_bg_color( rectangle, lv_color_make( r, g, b ), LV_PART_MAIN );
  lv_obj_set_style_bg_opa( rectangle, LV_OPA_COVER, LV_PART_MAIN );

  /* If all sliders are at 255 value then load the next screen */
  if ( r == 255 && g == 255 && b == 255 )
  {
    gui_cfg_mng_process( GUI_MNG_EV_LOAD_SENSOR_SCREEN, NULL );
  }
}

/**
 * @brief Build a simple LVGL starter screen.
 *
 * @param data Pointer to event data structure.
 */
static void gui_startup( const gui_mng_event_data_t *data )
{
  (void)data;
  lv_obj_t *screen = main_screen_create();
  lv_screen_load( screen );

  /* setting up rgb mixer callback */
  /* get the rectange object pointer using its name */
  lv_obj_t * rectangle = lv_obj_get_child_by_name( screen, "rgb_mix_rectangle" );
  if ( rectangle != NULL )
  {
    /* The subjects are plain global variables exported by ui_gen.h.
     * No lookup function is needed - just take their address directly. */
    lv_subject_add_observer_obj( &red_val,   gui_startup_rgb_obsv_cb, rectangle, NULL );
    lv_subject_add_observer_obj( &green_val, gui_startup_rgb_obsv_cb, rectangle, NULL );
    lv_subject_add_observer_obj( &blue_val,  gui_startup_rgb_obsv_cb, rectangle, NULL );
  }
}

/**
 * @brief Load the sensor screen when the corresponding event is received.
 * @param data Pointer to event data structure.
 */
static void gui_load_sensor_screen( const gui_mng_event_data_t *data )
{
  (void)data;
  /* Load the sensor screen */
  active_sensor_screen = sensor_screen_create();
  /* Load with a smooth fade-in animation and auto-delete old screen to free RAM */
  lv_screen_load_anim( active_sensor_screen, LV_SCR_LOAD_ANIM_FADE_ON, 400, 100, true );
}

/**
 * @brief Update the sensor data on the active sensor screen.
 * @param data Pointer to event data structure.
 */
static void gui_sensor_data_update( const gui_mng_event_data_t *data )
{
  if ( (active_sensor_screen == NULL) || (data == NULL) )
  {
    return; /* No active sensor screen or no data to update */
  }

  char buf[32];
  /* Temperature: Value & Status */
  int32_t t_int = data->sensor_data.temperature / 100;
  int32_t t_dec = (data->sensor_data.temperature % 100) / 10;
  snprintf( buf, sizeof(buf), "%ld.%ld °C", t_int, (t_dec < 0 ? -t_dec : t_dec) );
  lv_subject_copy_string( &temp_str, buf );
  const sensor_range_rule_t *t_rule = get_sensor_rule( t_int, temp_rules, NUM_ELEMENTS(temp_rules) );
  lv_subject_copy_string( &temp_status_str, t_rule->text );

  /* Humidity: Value & Status */
  uint32_t hum_pct = data->sensor_data.humidity >> 10;
  snprintf( buf, sizeof(buf), "%lu %%", hum_pct );
  lv_subject_copy_string( &hum_str, buf );
  const sensor_range_rule_t *h_rule = get_sensor_rule( (int32_t)hum_pct, hum_rules, NUM_ELEMENTS(hum_rules) );
  lv_subject_copy_string( &hum_status_str, h_rule->text );

  /* Pressure: Value & Status */
  uint32_t press_hpa = data->sensor_data.pressure / 100;
  snprintf( buf, sizeof(buf), "%lu\nhPa", press_hpa );
  lv_subject_copy_string( &press_str, buf );
  const sensor_range_rule_t *p_rule = get_sensor_rule( (int32_t)press_hpa, press_rules, NUM_ELEMENTS(press_rules) );
  lv_subject_copy_string( &press_status_str, p_rule->text );

  /* Updating Status Label Colors and Box Color Logic */
  /* Cards Row Container */
  lv_obj_t *cards_row = lv_obj_get_child( active_sensor_screen, 1 );
  if ( cards_row != NULL )
  {
    /* temperature status coloring logic */
    lv_obj_t *temp_card = lv_obj_get_child( cards_row, 0 );
    if ( temp_card )
    {
      lv_obj_t *box = lv_obj_get_child_by_name( temp_card, "card_box" );
      lv_obj_t *lbl = lv_obj_get_child_by_name( box, "status_label" );
      if ( lbl ) 
      {
        lv_obj_set_style_text_color( lbl, lv_color_hex( t_rule->color ), LV_PART_MAIN );
      }
      if ( box ) 
      {
        lv_obj_set_style_border_color( box, lv_color_hex( t_rule->color ), LV_PART_MAIN );
        lv_obj_set_style_shadow_color( box, lv_color_hex( t_rule->color ), LV_PART_MAIN );
      }
    }

    /* humidity status coloring logic */
    lv_obj_t *hum_card = lv_obj_get_child( cards_row, 1 );
    if ( hum_card )
    {
      lv_obj_t *box = lv_obj_get_child_by_name( hum_card, "card_box" );
      lv_obj_t *lbl = lv_obj_get_child_by_name( box, "status_label" );
      if ( lbl ) 
      {
        lv_obj_set_style_text_color( lbl, lv_color_hex( h_rule->color ), LV_PART_MAIN );
      }
      if ( box ) 
      {
        lv_obj_set_style_border_color( box, lv_color_hex( h_rule->color ), LV_PART_MAIN );
        lv_obj_set_style_shadow_color( box, lv_color_hex( h_rule->color ), LV_PART_MAIN );
      }
    }

    /* pressure status coloring logic */
    lv_obj_t *press_card = lv_obj_get_child( cards_row, 2 );
    if ( press_card )
    {
      lv_obj_t *box = lv_obj_get_child_by_name( press_card, "card_box" );
      lv_obj_t *lbl = lv_obj_get_child_by_name( box, "status_label" );
      if ( lbl ) 
      {
        lv_obj_set_style_text_color( lbl, lv_color_hex( p_rule->color ), LV_PART_MAIN );
      }
      if ( box ) 
      {
        lv_obj_set_style_border_color( box, lv_color_hex( p_rule->color ), LV_PART_MAIN );
        lv_obj_set_style_shadow_color( box, lv_color_hex( p_rule->color ), LV_PART_MAIN );
      }
    }
  }
}

