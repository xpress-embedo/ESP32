/*
 * gui_mng_cfg.c
 *
 *  Created on: Jun 15, 2024
 *      Author: xpress_embedo
 */

#include "main.h"
#include "lvgl.h"
#include "gui_mng.h"

// Private Variables
static lv_obj_t * ui_lblHeadLine;
static lv_obj_t * ui_lblTemperature;
static lv_obj_t * ui_lblTemperatureValue;
static lv_obj_t * ui_lblHumidity;
static lv_obj_t * ui_lblHumidityValue;
static lv_obj_t * ui_chart;
static lv_chart_series_t * temp_series;
static lv_chart_series_t * humid_series;

// Public Function Definitions
void gui_cfg_init( void )
{
  sensor_data_t *sensor_data = get_temperature_humidity();
  uint8_t *temp_data = sensor_data->temperature;
  uint8_t *humid_data = sensor_data->humidity;

  uint16_t disp_width = lv_disp_get_hor_res(NULL);
  uint16_t disp_height = lv_disp_get_ver_res(NULL);
  LV_LOG_USER("Display Width %d", disp_width);
  LV_LOG_USER("Display Height %d", disp_height);

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
  lv_obj_align_to(ui_lblHumidity, ui_lblHeadLine, LV_ALIGN_BOTTOM_MID, 200, 50);
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

  /*
  // Create a chart
  ui_chart = lv_chart_create( lv_scr_act() );
  lv_obj_set_size(ui_chart, (disp_width*2)/3, (disp_height*2)/3 );
  lv_obj_center(ui_chart);

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
  */
}
