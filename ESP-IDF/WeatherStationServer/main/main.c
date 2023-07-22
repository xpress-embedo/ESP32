#include "prj_refs.h"
#include "nvs_flash.h"
#include "led_strip.h"
#include "esp_random.h"         // Temporary Use, once I will receive the sensor, I will remove this
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "led_mng.h"
#include "wifi_app.h"

// Macros
#define MAIN_TASK_PERIOD            (10000)

// Private Variables
static const char TAG[] = "main";
static uint8_t humidity = 0u;
static uint8_t temperature = 0u;

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

  while (true)
  {
    temperature = 25u + (uint8_t)(esp_random() % 5);
    temperature = 50u + (uint8_t)(esp_random() % 4);
    ESP_LOGI(TAG, "Temperature: %d, Humidity: %d", temperature, humidity);
    vTaskDelay(MAIN_TASK_PERIOD / portTICK_PERIOD_MS);
  }
}

