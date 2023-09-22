/*
 * wifi_reset_button.c
 *
 *  Created on: 22-Sep-2023
 *      Author: xpress_embedo
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "tasks_common.h"
#include "wifi_app.h"
#include "wifi_reset_button.h"

// Private Variables
static const char TAG[] = "WiFi Reset Button";

// Semaphore Handle
SemaphoreHandle_t wifi_reset_semaphore = NULL;

/*
 * ISR Handler for the WiFi Reset (Boot) Button
 * @param arg parameter which can be passed to the ISR handler
 */
void IRAM_ATTR wifi_reset_button_isr_handler( void *arg )
{
  // Notify the button task
  xSemaphoreGiveFromISR(wifi_reset_semaphore, NULL);
}

// Private Function Declaration
void wifi_reset_button_task( void *pvParam );

// Public Function Definition
/*
 * Configures the WiFi Reset Button and Interrupt Configuration
 */
void wifi_reset_button_config( void )
{
  // Create Binary Semaphore
  wifi_reset_semaphore = xSemaphoreCreateBinary();

  // Configure the button and set the direction
  esp_rom_gpio_pad_select_gpio(WIFI_RESET_BUTTON);
  gpio_set_direction(WIFI_RESET_BUTTON, GPIO_MODE_INPUT);

  // Enable the interrupt on the Negative Edge
  gpio_set_intr_type(WIFI_RESET_BUTTON, GPIO_INTR_NEGEDGE);

  // Create the WiFi Reset Button Task
  xTaskCreate(&wifi_reset_button_task, "WiFi Reset Button", WIFI_RESET_BUTTON_TASK_STACK_SIZE, NULL, WIFI_RESET_BUTTON_TASK_PRIORITY, NULL);

  // Install GPIO ISR Service
  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

  // Attach the Interrupt Service Routine
  gpio_isr_handler_add(WIFI_RESET_BUTTON, wifi_reset_button_isr_handler, NULL);
}

// Private Function Definition
/*
 * WiFi Reset button task reacts to a BOOT button every event by sending a message
 * to the WiFi Application to disconnect from the WiFi and clear the saved credentials
 * @param pvParam parameter which can be passed to the task
 */
void wifi_reset_button_task( void *pvParam )
{
  for(;;)
  {
    if( xSemaphoreTake(wifi_reset_semaphore, portMAX_DELAY) == pdTRUE )
    {
      ESP_LOGI(TAG, "WiFi Reset Button Interrupt Occurred");

      // Send Message to disconnect WiFi and Clear Credentials
      wifi_app_send_msg(WIFI_APP_MSG_USR_REQUESTED_STA_DISCONNECT);
      // This delay is added to prevent re-triggering very soon, a type of debounce
      vTaskDelay(2000/portTICK_PERIOD_MS);
    }
  }
}
