#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

/* Private Macros */
#define MAIN_TASK_PERIOD                                    (1000u)     // Main task period in milliseconds

/* Private Variables */
static const char *TAG = "MAIN";

void app_main(void)
{
  ESP_LOGI( TAG, "Program Started" );
  while(1)
  {
    ESP_LOGI( TAG, "Hello World" );
    vTaskDelay( pdMS_TO_TICKS( 1000 ) );
  }
}
