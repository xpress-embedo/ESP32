#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "rgb_led.h"

void app_main(void)
{
  RGB_PWM_Init();
  while (true)
  {
    printf("Hello from app_main!\n");
    RGB_WiFi_Started();
    vTaskDelay(2000/portTICK_PERIOD_MS);
    RGB_HTTP_ServerStarted();
    vTaskDelay(2000/portTICK_PERIOD_MS);
    RGB_WiFi_Connected();
    vTaskDelay(2000/portTICK_PERIOD_MS);
  }
}
