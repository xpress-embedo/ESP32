#include "prj_refs.h"
#include "nvs_flash.h"
#include "led_mng.h"
#include "wifi_app.h"

#include "led_strip.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Macros
#define MAIN_TASK_PERIOD            (10000)

// Private Variables

// Private Function Prototypes

void app_main(void)
{
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // initialize led manager module
  led_init();

  // Start WiFi
  wifi_app_start();

//  while (true)
//  {
//    printf("Hello from app_main!\n");
//    vTaskDelay(MAIN_TASK_PERIOD / portTICK_PERIOD_MS);
//  }
}

