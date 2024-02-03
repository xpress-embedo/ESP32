#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"
#include "display_mng.h"
#include "dht11.h"
#include "ui.h"

typedef enum {
  LANGUAGE_ENGLISH = 0,
  LANGUAGE_GERMAN,
  LANGUAGE_HINDI,
  LANGUAGE_MAX
} language_t;

// macros
#define DHT11_PIN                     (GPIO_NUM_12)
#define MAIN_TASK_PERIOD              (5000)

// Private Variables
static const char *TAG = "APP";
static int temperature = 0;
static int humidity = 0;
static language_t current_language = LANGUAGE_ENGLISH;

const static char * welcome_label[LANGUAGE_MAX] =
{
  "Welcome to the Embedded Laboratory",
  "Willkommen im Embedded Laboratory",
  "एम्बेडेड प्रयोगशाला में आपका स्वागत है",
};

const static char * temperature_label[LANGUAGE_MAX] =
{
  "Temperature",
  "Temperatur",
  "तापमान",
};

const static char * humidity_label[LANGUAGE_MAX] =
{
  "Humidity",
  "Feuchtigkeit",
  "नमी",
};

// Private Function Declarations
static void update_temperature_humidity( void );
static void update_labels( language_t language );

void app_main(void)
{
  // Disable default gpio logging messages
  esp_log_level_set("gpio", ESP_LOG_NONE);

  // initialize dht sensor library
  dht11_init(DHT11_PIN);

  // initialize display related stuff, also lvgl
  display_init();

  // main user interface
  ui_init();
  current_language = LANGUAGE_ENGLISH;
  // update_labels(current_language);

  while (true)
  {
    vTaskDelay(MAIN_TASK_PERIOD / portTICK_PERIOD_MS);
    // Get DHT11 Temperature and Humidity Values
    if( dht11_read().status == DHT11_OK )
    {
      temperature = (uint8_t)dht11_read().temperature;
      humidity = (uint8_t)dht11_read().humidity;
      ESP_LOGI(TAG, "Temperature: %d", temperature);
      ESP_LOGI(TAG, "Humidity: %d", humidity);
      update_temperature_humidity();
    }
    else
    {
      ESP_LOGI(TAG, "Unable to Read DHT11 Status");
    }
  }
}

void update_language(lv_event_t * e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = lv_event_get_target(e);
  uint16_t selected_idx = 0;
  if( LV_EVENT_VALUE_CHANGED == code )
  {
    selected_idx = lv_dropdown_get_selected(obj);
    if( selected_idx < LANGUAGE_MAX )
    {
      ESP_LOGI(TAG, "Value Change Event, %d", selected_idx);
      current_language = (language_t)(selected_idx);
      update_labels(current_language);
    }
  }
}

// Private Function Definitions
/*
 * @brief Update the temperature and humidity values of the display
 */
static void update_temperature_humidity( void )
{
  if( true == display_update_lock() )
  {
    lv_label_set_text_fmt(ui_lblTemperatureValue, "%d °C", temperature);
    lv_label_set_text_fmt(ui_lblHumidityValue, "%d %%", humidity);
    // ESP_LOGI(TAG, "Variable Temperature = %d, Humidity = %d Updated on Display", temperature, humidity);
    // ESP_LOGI(TAG, "Label => Temperature = %s, Humidity = %s", lv_label_get_text(ui_lblTemperatureValue), lv_label_get_text(ui_lblHumidityValue));
    display_update_unlock();
  }
}

static void update_labels( language_t language )
{
  if( language < LANGUAGE_MAX )
  {
    if( true == display_update_lock() )
    {
      switch (language)
      {
        case LANGUAGE_ENGLISH:
        case LANGUAGE_GERMAN:
          lv_obj_set_style_text_font(ui_lblTemperature, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
          lv_obj_set_style_text_font(ui_lblHumidity, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
          lv_obj_set_style_text_font(ui_lblHeadLine, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
          break;
        case LANGUAGE_HINDI:
          lv_obj_set_style_text_font(ui_lblTemperature, &ui_font_mangal18, LV_PART_MAIN | LV_STATE_DEFAULT);
          lv_obj_set_style_text_font(ui_lblHumidity, &ui_font_mangal18, LV_PART_MAIN | LV_STATE_DEFAULT);
          lv_obj_set_style_text_font(ui_lblHeadLine, &ui_font_mangal24, LV_PART_MAIN | LV_STATE_DEFAULT);
          break;
        default:
          break;
      }
      lv_label_set_text_static(ui_lblTemperature, temperature_label[language]);
      lv_label_set_text_static(ui_lblHumidity, humidity_label[language]);
      lv_label_set_text_static(ui_lblHeadLine, welcome_label[language]);
      display_update_unlock();
    }
  }
}

