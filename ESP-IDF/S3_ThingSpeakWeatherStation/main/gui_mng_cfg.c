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
static void gui_update_sensor_data( uint8_t *data );

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
  { GUI_MNG_EV_TEMP_HUMID,          gui_update_sensor_data  },
};

// Public Function Definitions

/**
 * @brief GUI Configurable Initialization Function
 * @param  None
 */
void gui_cfg_init( void )
{
  sensor_data_t *sensor_data = get_temperature_humidity();
  uint8_t *temp_data = sensor_data->temperature;
  uint8_t *humid_data = sensor_data->humidity;

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

  // Create a chart
  ui_chart = lv_chart_create( lv_scr_act() );
  lv_obj_set_size(ui_chart, lv_pct(85), lv_pct(70) );
  lv_obj_center(ui_chart);
  lv_obj_set_pos( ui_chart, 0, 30 );
  lv_chart_set_type(ui_chart, LV_CHART_TYPE_LINE);
  lv_chart_set_range(ui_chart, LV_CHART_AXIS_PRIMARY_Y, 10, 60);
  lv_chart_set_range(ui_chart, LV_CHART_AXIS_SECONDARY_Y, 20, 100);
  // Tick Marks and Labels
  // 2nd argument is axis, 3rd argument is major tick length, 4th is minor tick length
  // 5th is number of major ticks on the axis
  // 6th is number of minor ticks between two major ticks
  // 7th is enable label drawing on major ticks
  // 8th is extra size required to draw labels and ticks
  lv_chart_set_axis_tick(ui_chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 0, 2, false, 50);
  lv_chart_set_axis_tick(ui_chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 2, true, 50);
  lv_chart_set_axis_tick(ui_chart, LV_CHART_AXIS_SECONDARY_Y, 10, 5, 5, 2, true, 25);

  // this should match with the temperature & humidity buffer length
  // NOTE: if generating using the Square Line Studio, make sure it matches the same value
  uint16_t chart_hor_res = SENSOR_BUFF_SIZE;
  // By default the number of points are 10, update it to chart width
  lv_chart_set_point_count( ui_chart, chart_hor_res );
  // Do not display points on the data
  lv_obj_set_style_size( ui_chart, 0, LV_PART_INDICATOR);
  // Update mode shift or circular, here shift is selected
  lv_chart_set_update_mode( ui_chart, LV_CHART_UPDATE_MODE_SHIFT );

  // Add data series for temperature on primary y-axis
  temp_series = lv_chart_add_series(ui_chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);
  // Add data series for humidity on secondary y-axis
  humid_series = lv_chart_add_series(ui_chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_SECONDARY_Y);

  for( size_t idx=0; idx<chart_hor_res; idx++ )
  {
    temp_series->y_points[idx] = (lv_coord_t)*(temp_data+idx);
    humid_series->y_points[idx] = (lv_coord_t)*(humid_data+idx);
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

// Private Function Definitions
/**
 * @brief Update the Temperature and Humidity data on display
 * @param data pointer to sensor data
 */
static void gui_update_sensor_data( uint8_t *data )
{
  sensor_data_t *sensor_data;
  sensor_data = (sensor_data_t*)data;
  lv_label_set_text_fmt(ui_lblTemperatureValue, "%d °C", sensor_data->temperature_current );
  lv_label_set_text_fmt(ui_lblHumidityValue, "%d %%", sensor_data->humidity_current );

  // this should match the temperature buffer length
  uint16_t chart_hor_res = SENSOR_BUFF_SIZE;
  size_t idx = 0;

  for( idx=0; idx<chart_hor_res; idx++ )
  {
    temp_series->y_points[idx] = (sensor_data->temperature[idx]);
    humid_series->y_points[idx] = (sensor_data->humidity[idx]);
  }
  lv_chart_refresh(ui_chart);
}
