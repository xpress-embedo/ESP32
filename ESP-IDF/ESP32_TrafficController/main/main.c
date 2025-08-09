/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "esp_event.h"
#include "esp_log.h"

#include "nvs_flash.h"

#include "main.h"
#include "wifi_app.h"
#include "gui_mng.h"
#include "mqtt_app.h"

// Private Macros
#define MAIN_TASK_PERIOD                    (1000)

// Private Variables
static const char *TAG = "APP";

// Private Function Declarations
// todo

void app_main(void)
{
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_LOGI(TAG, "Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
  ESP_LOGI(TAG, "IDF version: %s", esp_get_idf_version());

  esp_log_level_set("gpio", ESP_LOG_NONE);

  // start the GUI manager
  gui_start();

  // send an event to GUI manager
  gui_send_event(GUI_MNG_EV_WIFI_CONNECTING, NULL);

  // start wifi application (Soft AP and HTTP Web Server)
  wifi_app_start();

  // start the mqtt task (will not initiate the connection with server until
  // wifi connection is established
  mqtt_app_start();

  // connect with mqtt server if connection is successful
  if( 0 )
  {
    // send an event to GUI manager
    gui_send_event(GUI_MNG_EV_MQTT_CONNECTING, NULL);

  }

  while (true )
  {
    // Wait before next
    vTaskDelay(MAIN_TASK_PERIOD / portTICK_PERIOD_MS);
  }
}

// Private Function Definitions

