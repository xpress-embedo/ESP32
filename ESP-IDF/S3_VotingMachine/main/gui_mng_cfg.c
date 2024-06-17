/*
 * gui_mng_cfg.c
 *
 *  Created on: Jun 15, 2024
 *      Author: xpress_embedo
 */

#include "main.h"
#include "lvgl.h"
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

// Private Function Prototypes


// Private Variables
static lv_obj_t * ui_lblHeadLine;
static lv_obj_t * ui_lblTemperature;
static lv_obj_t * ui_lblTemperatureValue;
static lv_obj_t * ui_lblHumidity;
static lv_obj_t * ui_lblHumidityValue;
static lv_obj_t * ui_chart;
static lv_chart_series_t * temp_series;
static lv_chart_series_t * humid_series;

static const gui_mng_event_cb_t gui_mng_event_cb[] =
{
};

// Public Function Definitions

/**
 * @brief GUI Configurable Initialization Function
 * @param  None
 */
void gui_cfg_init( void )
{
  // uint16_t disp_width = lv_disp_get_hor_res(NULL);
  // uint16_t disp_height = lv_disp_get_ver_res(NULL);
  // LV_LOG_USER("Display Width %d", disp_width);
  // LV_LOG_USER("Display Height %d", disp_height);

  // make screen non-scrollable
  lv_obj_clear_flag( lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE );

  // Headline or let's say title starts
  ui_lblHeadLine = lv_label_create( lv_scr_act() );
  // lv_obj_set_width(ui_lblHeadLine, disp_width);
  // NOTE: instead of using display_width we can use lv_pct(100), which means 100% of display width
  lv_obj_set_width(ui_lblHeadLine, lv_pct(100));
  lv_obj_set_height(ui_lblHeadLine, 40);
  lv_label_set_text(ui_lblHeadLine, "Temperature and Humidity Graph.");
  lv_obj_set_style_text_color(ui_lblHeadLine, lv_color_hex(0x084146), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(ui_lblHeadLine, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui_lblHeadLine, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_align(ui_lblHeadLine, LV_ALIGN_CENTER);
  lv_obj_set_x(ui_lblHeadLine, 0);
  lv_obj_set_y(ui_lblHeadLine, -220);
  // Headline ends

  // Temperature and Humidity Labels with Values
  ui_lblHumidity = lv_label_create( lv_scr_act() );
  lv_label_set_text(ui_lblHumidity, "Humidity");
  lv_obj_set_style_text_font(ui_lblHumidity, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_width(ui_lblHumidity, 200);
  lv_obj_set_height(ui_lblHumidity, 30);
  lv_obj_align_to(ui_lblHumidity, ui_lblHeadLine, LV_ALIGN_BOTTOM_MID, 200, 30);
  // NOTE: Instead of using set_x and set_y function, I used the align_to function
  // this helps in aligning things properly, and when using this lv_obj_set_align is not needed
  // lv_obj_set_align(ui_lblHumidity, LV_ALIGN_CENTER);
  // lv_obj_set_x(ui_lblHumidity, 100);
  // lv_obj_set_y(ui_lblHumidity, -100);

  ui_lblHumidityValue = lv_label_create( lv_scr_act() );
  lv_label_set_text(ui_lblHumidityValue, "0 %");
  lv_obj_set_style_text_font(ui_lblHumidityValue, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_width(ui_lblHumidityValue, 200);
  lv_obj_set_height(ui_lblHumidityValue, 30);
  lv_obj_align_to(ui_lblHumidityValue, ui_lblHumidity, LV_ALIGN_CENTER, 200, 0);
  // lv_obj_set_align(ui_lblHumidityValue, LV_ALIGN_CENTER);
  // lv_obj_set_x(ui_lblHumidityValue, 270);
  // lv_obj_set_y(ui_lblHumidityValue, -100);

  ui_lblTemperature = lv_label_create( lv_scr_act() );
  lv_label_set_text(ui_lblTemperature, "Temperature");
  lv_obj_set_style_text_font(ui_lblTemperature, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_width(ui_lblTemperature, 200);
  lv_obj_set_height(ui_lblTemperature, 30);
  lv_obj_align_to(ui_lblTemperature, ui_lblHumidity, LV_ALIGN_CENTER, 0, 30);
  // lv_obj_set_align(ui_lblTemperature, LV_ALIGN_CENTER);
  // lv_obj_set_x(ui_lblTemperature, 100);
  // lv_obj_set_y(ui_lblTemperature, 0);

  ui_lblTemperatureValue = lv_label_create( lv_scr_act() );
  lv_label_set_text(ui_lblTemperatureValue, "0 °C");
  lv_obj_set_style_text_font(ui_lblTemperatureValue, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_width(ui_lblTemperatureValue, 200);
  lv_obj_set_height(ui_lblTemperatureValue, 30);
  lv_obj_align_to(ui_lblTemperatureValue, ui_lblTemperature, LV_ALIGN_CENTER, 200, 0);
  // lv_obj_set_align(ui_lblTemperatureValue, LV_ALIGN_CENTER);
  // lv_obj_set_x(ui_lblTemperatureValue, 270);
  // lv_obj_set_y(ui_lblTemperatureValue, 0);

  // Legend Related Code
  // Showing Legends Code Starts from Here: this is temporary stuff, not the
  // proper way of doing things, & library also doesn't support showing legends
  // Create an array of Points for Lines
  static lv_point_t temp_points[] = { {0,0}, {100,0} };
  // Create Style for temperature and humidity legends
  static lv_style_t style_line_temp;
  lv_style_init(&style_line_temp);
  lv_style_set_line_width(&style_line_temp, 4);
  lv_style_set_line_color(&style_line_temp, lv_palette_main(LV_PALETTE_BLUE));

  static lv_style_t style_line_humid;
  lv_style_init(&style_line_humid);
  lv_style_set_line_width(&style_line_humid, 4);
  lv_style_set_line_color(&style_line_humid, lv_palette_main(LV_PALETTE_GREEN));

  // Create a line and apply the style
  lv_obj_t * temp_line;
  lv_obj_t * humid_line;
  temp_line = lv_line_create( lv_scr_act() );
  humid_line = lv_line_create( lv_scr_act() );
  // set the points for temperature legend
  lv_line_set_points(temp_line, temp_points, 2);
  lv_obj_add_style(temp_line, &style_line_temp, 0);
  lv_obj_align_to(temp_line, ui_lblTemperature, LV_ALIGN_CENTER, -160, 0);
  // set the points for humidity legend
  lv_line_set_points(humid_line, temp_points, 2); // Using the same temp points
  lv_obj_add_style(humid_line, &style_line_humid, 0);
  lv_obj_align_to(humid_line, ui_lblHumidity, LV_ALIGN_CENTER, -160, 0);
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

// Private Function Definitions

