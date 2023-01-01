/*
 * Display Manager.c
 *
 *  Created on: 01-Nov-2022
 *      Author: xpress_embedo
 */
#include "display_mng.h"
#include <Arduino.h>
#include <lvgl.h>

typedef enum _Display_State_e
{
  DISP_STATE_VIBGYOR = 0,
  DISP_STATE_VIBGYOR_WAIT,
  DISP_STATE_RGB_MIXER,
  DISP_STATE_RGB_MIXER_WAIT,
  DISP_STATE_TEMP_SENSOR,
  DISP_STATE_TEMP_SENSOR_REFRESH,
  DISP_STATE_END,
} Display_State_e;

typedef enum _SliderType_e
{
  SLIDER_TYPE_RED = 0,
  SLIDER_TYPE_GREEN,
  SLIDER_TYPE_BLUE,
} SliderType_e;

typedef struct _RGB_Mixer_s
{
  SliderType_e  slider_type;
  lv_obj_t*     label;
} RGB_Mixer_s;

/*---------------------------Private Variables--------------------------------*/
static Display_State_e disp_state = DISP_STATE_VIBGYOR;
static RGB_Mixer_s red, green, blue;
static lv_obj_t *rectangle;
static lv_style_t style;
// for temperature chart
static lv_obj_t * chart;
static lv_chart_series_t * temp_series;

/*--------------------------Private Function Prototypes-----------------------*/
static void Display_Vibgyor( void );
static void Display_RGBMixer( void );
static void Display_TemperatureChart( void );
static void Display_TemperatureChartRefresh( void );
static void Slider_Callback( lv_event_t *e );

/*---------------------------Public Function Definitions----------------------*/
void Display_Mng( void )
{
  static uint32_t wait_time = 0u;
  switch( disp_state )
  {
    case DISP_STATE_VIBGYOR:
      Display_Vibgyor();
      wait_time = millis();
      disp_state = DISP_STATE_VIBGYOR_WAIT;
      break;
    case DISP_STATE_VIBGYOR_WAIT:
      // wait here for some time and then move to next state
      if( millis()-wait_time > 4000u )
      {
        disp_state = DISP_STATE_RGB_MIXER;
      }
      break;
    case DISP_STATE_RGB_MIXER:
      Display_RGBMixer();
      disp_state = DISP_STATE_RGB_MIXER_WAIT;
      break;
    case DISP_STATE_RGB_MIXER_WAIT:
      // state will be switched to next state automatically from the callback function
      break;
    case DISP_STATE_TEMP_SENSOR:
      Display_TemperatureChart();
      disp_state = DISP_STATE_TEMP_SENSOR_REFRESH;
      wait_time = millis();
      break;
    case DISP_STATE_TEMP_SENSOR_REFRESH:
      if( millis()-wait_time > 1000u )
      {
        wait_time = millis();
        // Note: Charts are time consuming
        Display_TemperatureChartRefresh();
      }
      break;
    case DISP_STATE_END:
      break;
  };
}


static void Display_Vibgyor( void )
{
  static lv_style_t style;
  lv_coord_t width = 0u;
  lv_coord_t length = 0u;

  lv_obj_t * V_rectangle;
  lv_obj_t * I_rectangle;
  lv_obj_t * B_rectangle;
  lv_obj_t * G_rectangle;
  lv_obj_t * Y_rectangle;
  lv_obj_t * O_rectangle;
  lv_obj_t * R_rectangle;

  lv_obj_t *act_scr = lv_scr_act();           // Get the active screen object

  R_rectangle = lv_obj_create( act_scr );     // Create Rectangle Object
  O_rectangle = lv_obj_create( act_scr );
  Y_rectangle = lv_obj_create( act_scr );
  G_rectangle = lv_obj_create( act_scr );
  B_rectangle = lv_obj_create( act_scr );
  I_rectangle = lv_obj_create( act_scr );
  V_rectangle = lv_obj_create( act_scr );

  lv_style_init(&style);
  // set the radius to zero
  lv_style_set_radius(&style, 0);
  // by default the object which we created for rectangle has some radius component
  // and it looks bad for this particular example, hence updating style for all
  // created objects
  lv_obj_add_style(R_rectangle, &style, 0);
  lv_obj_add_style(O_rectangle, &style, 0);
  lv_obj_add_style(Y_rectangle, &style, 0);
  lv_obj_add_style(G_rectangle, &style, 0);
  lv_obj_add_style(B_rectangle, &style, 0);
  lv_obj_add_style(I_rectangle, &style, 0);
  lv_obj_add_style(V_rectangle, &style, 0);


  length = lv_disp_get_hor_res(NULL);
  // VIBGYOR are seven colors
  width = lv_disp_get_physical_ver_res(NULL)/7;

  lv_obj_set_size(R_rectangle, length, width);
  lv_obj_align(R_rectangle, LV_ALIGN_TOP_LEFT, 0, 0 );
  lv_obj_set_style_border_color(R_rectangle, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN );
  lv_obj_set_style_bg_color( R_rectangle, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN );

  lv_obj_set_size(O_rectangle, length, width );
  // lv_obj_align_to(O_rectangle, R_rectangle, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_align(O_rectangle, LV_ALIGN_TOP_LEFT, 0, width );
  lv_obj_set_style_border_color(O_rectangle, lv_palette_main(LV_PALETTE_ORANGE), LV_PART_MAIN );
  lv_obj_set_style_bg_color( O_rectangle, lv_palette_main(LV_PALETTE_ORANGE), LV_PART_MAIN );

  lv_obj_set_size(Y_rectangle, length, width );
  // lv_obj_align_to(Y_rectangle, O_rectangle, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_align(Y_rectangle, LV_ALIGN_TOP_LEFT, 0, width*2u );
  lv_obj_set_style_border_color(Y_rectangle, lv_palette_main(LV_PALETTE_YELLOW), LV_PART_MAIN );
  lv_obj_set_style_bg_color( Y_rectangle, lv_palette_main(LV_PALETTE_YELLOW), LV_PART_MAIN );

  lv_obj_set_size(G_rectangle, length, width );
  // lv_obj_align_to(G_rectangle, Y_rectangle, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_align(G_rectangle, LV_ALIGN_TOP_LEFT, 0, width*3u );
  lv_obj_set_style_border_color(G_rectangle, lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN );
  lv_obj_set_style_bg_color( G_rectangle, lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN );

  lv_obj_set_size(B_rectangle, length, width );
  // lv_obj_align_to(B_rectangle, G_rectangle, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_align(B_rectangle, LV_ALIGN_TOP_LEFT, 0, width*4u );
  lv_obj_set_style_border_color(B_rectangle, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN );
  lv_obj_set_style_bg_color( B_rectangle, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN );

  lv_obj_set_size(I_rectangle, length, width );
  // lv_obj_align_to(Y_rectangle, B_rectangle, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_align(I_rectangle, LV_ALIGN_TOP_LEFT, 0, width*5u );
  lv_obj_set_style_border_color(I_rectangle, lv_palette_main(LV_PALETTE_INDIGO), LV_PART_MAIN );
  lv_obj_set_style_bg_color( I_rectangle, lv_palette_main(LV_PALETTE_INDIGO), LV_PART_MAIN );

  lv_obj_set_size(V_rectangle, length, width );
  // lv_obj_align_to(V_rectangle, I_rectangle, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_align(V_rectangle, LV_ALIGN_TOP_LEFT, 0, width*6u );
  lv_obj_set_style_border_color(V_rectangle, lv_palette_main(LV_PALETTE_DEEP_PURPLE), LV_PART_MAIN );
  lv_obj_set_style_bg_color( V_rectangle, lv_palette_main(LV_PALETTE_DEEP_PURPLE), LV_PART_MAIN );
}


static void Display_RGBMixer( void )
{
  lv_obj_t *act_scr = lv_scr_act();                     // Get the active screen object

  lv_obj_clean( act_scr );                              // Clear the screen

  // intialize the styles
  lv_style_init(&style);
  // we have to enable other font sizes in menuconfig
  lv_style_set_text_font(&style, &lv_font_montserrat_32);

  // RED, Green and Blue Slider Configuration
  lv_obj_t *slider_r = lv_slider_create( act_scr );     // create a red slider base object
  lv_obj_t *slider_g = lv_slider_create( act_scr );     // create a green slider base object
  lv_obj_t *slider_b = lv_slider_create( act_scr );     // create a blue slider base object

  // Setting Sliders Width
  lv_obj_set_width( slider_r, LV_PCT(80) );
  lv_obj_set_width( slider_g, LV_PCT(80) );
  lv_obj_set_width( slider_b, LV_PCT(80) );

  // Setting Sliders Height
  lv_obj_set_height( slider_r, LV_PCT(4) );
  lv_obj_set_height( slider_g, LV_PCT(4) );
  lv_obj_set_height( slider_b, LV_PCT(4) );

  // Align Sliders with Each Other
  lv_obj_align( slider_r, LV_ALIGN_TOP_MID, 0u, LV_PCT(20) );
  lv_obj_align_to( slider_g, slider_r, LV_ALIGN_TOP_MID, 0u, 70u );
  lv_obj_align_to( slider_b, slider_g, LV_ALIGN_TOP_MID, 0u, 70u );

  // set slider range also (by default it is 0 to 100 but we want till 255)
  lv_slider_set_range( slider_r, 0, 255 );
  lv_slider_set_range( slider_g, 0, 255 );
  lv_slider_set_range( slider_b, 0, 255 );

  // Coloring Sliders, Slider has three parts Main, Indicator and Knob
  // apply red color to the indicator part
  lv_obj_set_style_bg_color( slider_r, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR );
  // apply red color to the knob part
  lv_obj_set_style_bg_color( slider_r, lv_palette_main(LV_PALETTE_RED), LV_PART_KNOB );

  // apply green color to the indicator part
  lv_obj_set_style_bg_color( slider_g, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR );
  // apply green color to the knob part
  lv_obj_set_style_bg_color( slider_g, lv_palette_main(LV_PALETTE_GREEN), LV_PART_KNOB );

  // apply blue color to the indicator part
  lv_obj_set_style_bg_color( slider_b, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR );
  // apply blue color to the knob part
  lv_obj_set_style_bg_color( slider_b, lv_palette_main(LV_PALETTE_BLUE), LV_PART_KNOB );

  rectangle = lv_obj_create(act_scr);                   // Creates a base object Rectangle to display color
  lv_obj_set_size( rectangle, LV_PCT(93), LV_PCT(33) );
  lv_obj_align_to( rectangle, slider_b, LV_ALIGN_TOP_MID, 0u, 50u );
  lv_obj_set_style_border_color( rectangle, lv_color_black(), LV_PART_MAIN );   // add black border to rectangle
  lv_obj_set_style_border_width( rectangle, 2, LV_PART_MAIN );                  // increase the width of the border by 2px
  lv_obj_set_style_bg_color( rectangle, lv_color_make( 0, 0, 0), LV_PART_MAIN); // all sliders are at zero, so background color should be black

  // Create Main Heading Label
  lv_obj_t *heading = lv_label_create(act_scr);
  lv_label_set_text( heading, "RGB Mixer");
  lv_obj_align( heading, LV_ALIGN_TOP_MID, 0u, LV_PCT(5) );
  lv_obj_add_style( heading, &style, 0 );

  // Creating labels for individual slider current values
  red.slider_type = SLIDER_TYPE_RED;
  red.label = lv_label_create(act_scr);
  lv_label_set_text( red.label, "0");
  // lv_obj_align_to( red.label, slider_r, LV_ALIGN_TOP_MID, 0u, 0u );         // this will display inside slider
  lv_obj_align_to( red.label, slider_r, LV_ALIGN_OUT_TOP_MID, 0u, 0u );    // this will display outside slider

  green.slider_type = SLIDER_TYPE_GREEN;
  green.label = lv_label_create(act_scr);
  lv_label_set_text( green.label, "0");
  lv_obj_align_to( green.label, slider_g, LV_ALIGN_OUT_TOP_MID, 0u, 0u );

  blue.slider_type = SLIDER_TYPE_BLUE;
  blue.label = lv_label_create(act_scr);
  lv_label_set_text( blue.label, "0");
  lv_obj_align_to( blue.label, slider_b, LV_ALIGN_OUT_TOP_MID, 0u, 0u );

  // add event callbacks for sliders
  lv_obj_add_event_cb( slider_r, Slider_Callback, LV_EVENT_VALUE_CHANGED, &red );
  lv_obj_add_event_cb( slider_g, Slider_Callback, LV_EVENT_VALUE_CHANGED, &green );
  lv_obj_add_event_cb( slider_b, Slider_Callback, LV_EVENT_VALUE_CHANGED, &blue );
}

/*--------------------------Private Function Definitions----------------------*/
static void Slider_Callback( lv_event_t *e )
{
  static int32_t red, green, blue;
  int32_t slider_value = 0;
  // get the object (slider) for which the we received the event
  lv_obj_t *slider = lv_event_get_target(e);
  // extract the object (slider) user data
  RGB_Mixer_s *user_data = lv_event_get_user_data(e);
  // get the current slider value
  slider_value = lv_slider_get_value(slider);

  // now we have to update the slider labels
  lv_label_set_text_fmt( user_data->label, "%ld", slider_value );

  // now we have to update the color of the rectangle object based on the current
  // selected values of the color
  if( user_data->slider_type == SLIDER_TYPE_RED )
  {
    red = slider_value;
  }
  else if( user_data->slider_type == SLIDER_TYPE_GREEN )
  {
    green = slider_value;
  }
  else if( user_data->slider_type == SLIDER_TYPE_BLUE )
  {
    blue = slider_value;
  }
  // now we have the color information, update the rectangle color
  // NOTE: rectangle object must be file global else we will not be able to
  // update it from here
  lv_obj_set_style_bg_color( rectangle, lv_color_make( red, green, blue), LV_PART_MAIN);
  // the function `lv_color_make` can form colors if red, green and blue color
  // are specified

  if( (red == 255) && (green == 255) && (blue == 255) )
  {
    disp_state = DISP_STATE_TEMP_SENSOR;
  }
}

static void Display_TemperatureChart( void )
{
  uint16_t idx = 0u;
  // this should match with the temperature buffer length
  uint16_t chart_hor_res = 260;
  uint16_t chart_ver_res = lv_disp_get_ver_res(NULL) - 100;
  uint8_t *data = Display_GetTempData();

  lv_obj_clean( lv_scr_act() );                         // Clean the screen

  // Create a chart object
  chart = lv_chart_create( lv_scr_act() );

  // Create a label for Title text
  lv_obj_t * lbl_title = lv_label_create( lv_scr_act() );
  lv_label_set_text( lbl_title, "Temperature Graph");
  lv_obj_set_style_text_align( lbl_title, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align( lbl_title, LV_ALIGN_TOP_MID, 0, 0 );
  lv_obj_add_style( lbl_title, &style, 0 );

  // Set the chart size (Size should be set properly because we wanted to display
  // chart title and some data on y-axis also)
  // display is 320x240
  lv_obj_set_size( chart, (lv_disp_get_hor_res(NULL) - 100), chart_ver_res );
  // TODO: XS I don't want to center it, will check later
  // lv_obj_center( chart );
  lv_obj_align( chart, LV_ALIGN_CENTER, LV_PCT(5), 0 );
  // lv_obj_align( chart, LV_ALIGN_BOTTOM_RIGHT, 0, 0 );

  // Set Chart Type to Line Chart
  lv_chart_set_type( chart, LV_CHART_TYPE_LINE );
  // By Default the number of points are 10, update it to chart width
  lv_chart_set_point_count( chart, chart_hor_res );
  // Update mode shift or circular, here shift is selected
  lv_chart_set_update_mode( chart, LV_CHART_UPDATE_MODE_SHIFT );
  // Specify Vertical Range
  lv_chart_set_range( chart, LV_CHART_AXIS_PRIMARY_Y, 10, 60);
  // Tick Marks and Labels
  // 2nd argument is axis, 3rd argument is major tick length, 4th is minor tick length
  // 5th is number of major ticks on the axis
  // 6th is number of minor ticks between two major ticks
  // 7th is enable label drawing on major ticks
  // 8th is extra size required to draw labels and ticks
  lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 10, 2, true, 50);

  // Add Data Series
  temp_series = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);

  for( idx=0; idx<chart_hor_res; idx++ )
  {
    temp_series->y_points[idx] = (lv_coord_t)*(data+idx);
  }

  lv_chart_refresh(chart); /*Required after direct set*/
}

static void Display_TemperatureChartRefresh( void )
{
  uint16_t idx = 0u;
  uint8_t *data = Display_GetTempData();
  // this should match with the temperature buffer length
  uint16_t chart_hor_res = 260;

  for( idx=0; idx<chart_hor_res; idx++ )
  {
    temp_series->y_points[idx] = (lv_coord_t)*(data+idx);
  }

  lv_chart_refresh(chart); /*Required after direct set*/
}
