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

// Private Variables
static uint8_t total_num_of_cities = 0u;
static uint8_t city_idx = 0u;
static uint8_t display_refresh = 0;

// Public Function Definitions
void display_init(void)
{
  total_num_of_cities = openweathermap_get_numofcity();
  city_idx = 0u;

  // Start LVGL and LCD Driver
  bsp_display_start();
  bsp_display_lock(0);
  ui_init();
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
    // get the values from OpenWeatherMap
    temperature = openweathermap_get_temperature(city_idx);
    pressure = openweathermap_get_pressure(city_idx);
    humidity = openweathermap_get_humidity(city_idx);
    // Update the Display
    snprintf(temp,10u, "%2d C", temperature);
    _ui_label_set_property( ui_tempValue, _UI_LABEL_PROPERTY_TEXT, temp);
    snprintf(temp,10u, "%4d bar", pressure);
    _ui_label_set_property( ui_pressureValue, _UI_LABEL_PROPERTY_TEXT, temp);
    snprintf(temp,10u, "%3d ", humidity);
    _ui_label_set_property( ui_humidityValue, _UI_LABEL_PROPERTY_TEXT, temp);
  }
}
