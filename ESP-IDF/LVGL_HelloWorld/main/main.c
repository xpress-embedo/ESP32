#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "display_mng.h"

int16_t x = 0;
int16_t y = 0;

void app_main(void)
{
  display_init();

  while (true)
  {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Hello from app_main!\n");
    // lv_timer_handler();
  }
}
