/*
 * display_mng.c
 *
 *  Created on: 08-Jul-2023
 *      Author: xpress_embedo
 */

#include "display_mng.h"
#include "bsp/esp-bsp.h"
#include "openweathermap.h"
#include "ui.h"

// Macros
#define DISPLAY_REFRESH_RATE          (5u)    // display_mng is called after 1 second, using x means x seconds
#define NUM_OF_DATA                   (4u)    // This must be aligned with NUM_OF_CITIES in OpenWeatherMap module
                                              // and also should be equal to total_num_of_cities

// Private Variables
static uint8_t total_num_of_cities = 0u;
static uint8_t city_idx = 0u;
static uint8_t display_refresh = 0;
lv_obj_t * ui_Screens[NUM_OF_DATA];
lv_obj_t * ui_city_names[NUM_OF_DATA];
lv_obj_t * ui_temperature_values[NUM_OF_DATA];
lv_obj_t * ui_pressure_values[NUM_OF_DATA];
lv_obj_t * ui_humidity_values[NUM_OF_DATA];

// Public Function Definitions
void display_init(void)
{
  total_num_of_cities = openweathermap_get_numofcity();
  city_idx = 0u;

  // Start LVGL and LCD Driver
  bsp_display_start();
  bsp_display_lock(0);
  ui_init();
  // Update Array of Screen Pointers
  ui_Screens[0] = ui_Screen1;
  ui_Screens[1] = ui_Screen2;
  ui_Screens[2] = ui_Screen3;
  ui_Screens[3] = ui_Screen4;
  // Update Array of City Names with LVGL objects
  ui_city_names[0] = ui_cityNameValue0;
  ui_city_names[1] = ui_cityNameValue1;
  ui_city_names[2] = ui_cityNameValue2;
  ui_city_names[3] = ui_cityNameValue3;
  // Update Array of Temperature LVGL objects
  ui_temperature_values[0] = ui_tempValue0;
  ui_temperature_values[1] = ui_tempValue1;
  ui_temperature_values[2] = ui_tempValue2;
  ui_temperature_values[3] = ui_tempValue3;
  // Update Array of Pressure LVGL objects
  ui_pressure_values[0] = ui_pressureValue0;
  ui_pressure_values[1] = ui_pressureValue1;
  ui_pressure_values[2] = ui_pressureValue2;
  ui_pressure_values[3] = ui_pressureValue3;
  // Update Array of Humidity LVGL objects
  ui_humidity_values[0] = ui_humidityValue0;
  ui_humidity_values[1] = ui_humidityValue1;
  ui_humidity_values[2] = ui_humidityValue2;
  ui_humidity_values[3] = ui_humidityValue3;
  bsp_display_unlock();
}

// this function is called every 1 seconds
// and let's say we wanted to switch screen every 5 seconds
void display_mng(void)
{
  int temperature = 0;
  int humidity = 0;
  int pressure = 0;
  char temp[10] = {0};
  display_refresh++;
  if( display_refresh >= DISPLAY_REFRESH_RATE )
  {
    display_refresh = 0u;
    // Move to the next city
    city_idx++;
    if( city_idx >= total_num_of_cities )
    {
      city_idx = 0u;
    }
    // get the values from OpenWeatherMap
    temperature = openweathermap_get_temperature(city_idx);
    pressure = openweathermap_get_pressure(city_idx);
    humidity = openweathermap_get_humidity(city_idx);

    // load the screen
    lv_disp_load_scr( ui_Screens[city_idx] );
    // Update the Display
    snprintf(temp,10u, "%2d C", temperature);
    _ui_label_set_property( ui_temperature_values[city_idx], _UI_LABEL_PROPERTY_TEXT, temp);
    snprintf(temp,10u, "%4d bar", pressure);
    _ui_label_set_property( ui_pressure_values[city_idx], _UI_LABEL_PROPERTY_TEXT, temp);
    snprintf(temp,10u, "%3d ", humidity);
    _ui_label_set_property( ui_humidity_values[city_idx], _UI_LABEL_PROPERTY_TEXT, temp);
    _ui_label_set_property(ui_city_names[city_idx], _UI_LABEL_PROPERTY_TEXT, openweathermap_get_city_name(city_idx));
  }
}
