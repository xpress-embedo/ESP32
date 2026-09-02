#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "gui_mng.h"

/* Private Macros */
#define MAIN_TASK_PERIOD                                    (5000u)     // Main task period in milliseconds

/* Private Variables */
static const char *TAG = "MAIN";

void app_main(void)
{
  ESP_LOGI( TAG, "Program Started" );

  if ( !gui_start() )
  {
    ESP_LOGE( TAG, "GUI startup failed" );
    return;
  }

  while(1)
  {
    vTaskDelay( pdMS_TO_TICKS( MAIN_TASK_PERIOD ) );
  }
}
