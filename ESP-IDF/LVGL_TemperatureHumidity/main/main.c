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

// macros
#define DHT11_PIN                     (GPIO_NUM_12)
#define MAIN_TASK_PERIOD              (5000)

// Private Variables
static uint8_t button_counter = 0;
static lv_obj_t * count_label;
static const char *TAG = "APP";

// Private Function Declarations

void app_main(void)
{
  dht11_init(DHT11_PIN);
  display_init();
  

  while (true)
  {
    vTaskDelay(MAIN_TASK_PERIOD / portTICK_PERIOD_MS);
    /* Get DHT11 Temperature and Humidity Values */
    ESP_LOGI(TAG, "Temperature: %d", dht11_read().temperature);
    ESP_LOGI(TAG, "Humidity: %d", dht11_read().humidity);
    ESP_LOGI(TAG, "Status: %d", dht11_read().status);
  }
}


// Private Function Definitions
