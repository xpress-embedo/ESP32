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
#include "string.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_task_wdt.h"

#include "main.h"
#include "wifi_app.h"
#include "gui_mng.h"
#include "mqtt_app.h"

// Private Macros
#define MAIN_TASK_PERIOD                    (1000)
// GPIO Connections for LEDs
#define TRAFFIC_LED_01                      GPIO_NUM_14
#define TRAFFIC_LED_02                      GPIO_NUM_27
#define TRAFFIC_LED_03                      GPIO_NUM_26
#define TRAFFIC_LED_04                      GPIO_NUM_25
#define TRAFFIC_LED_05                      GPIO_NUM_33
#define TRAFFIC_LED_06                      GPIO_NUM_32
#define TRAFFIC_LED_07                      GPIO_NUM_13
#define TRAFFIC_LED_08                      GPIO_NUM_15
#define TRAFFIC_LED_09                      GPIO_NUM_4
#define TRAFFIC_LED_10                      GPIO_NUM_16
#define TRAFFIC_LED_11                      GPIO_NUM_17
#define TRAFFIC_LED_12                      GPIO_NUM_12   // there could be some issue with this pin

// GPIO22 is TXD and GPIO21 as RXD
#define UART_NUM                            UART_NUM_1
#define TXD_PIN                             GPIO_NUM_21
#define RXD_PIN                             GPIO_NUM_22
#define RX_BUFF_SIZE                        (100u)
#define PACKET_START                        '<'
#define PACKET_END                          '>'

typedef enum _rx_data_state_e
{
  RX_DATA_STATE_START,
  RX_DATA_STATE_COPY_DATA,
  RX_DATA_STATE_END,
} rx_data_state_e;

// Private Variables
static const char *TAG = "APP";
static uint8_t rx_buff[RX_BUFF_SIZE] = { 0 };
static uint8_t rx_buff_idx = 0;

// Private Function Declarations
static void uart_init( void );
static void uart_start( void );
static void uart_rx_task(void *pvParameter);

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

  // start uart for serial reception of data
  uart_start();

  const char *data = "Traffic Controller Starting\r\n";
  uart_write_bytes(UART_NUM, data, strlen(data));

  while (true )
  {
    // Wait before next
    vTaskDelay(MAIN_TASK_PERIOD / portTICK_PERIOD_MS);
  }
}

// Private Function Definitions
static void uart_init( void )
{
  const uart_config_t uart_config =
  {
      .baud_rate = 115200,
      .data_bits = UART_DATA_8_BITS,
      .parity    = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
  };

  uart_driver_install(UART_NUM, RX_BUFF_SIZE * 2, 0, 0, NULL, 0);
  uart_param_config(UART_NUM, &uart_config);
  uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

static void uart_start( void )
{
  uart_init();
  xTaskCreate(&uart_rx_task, "uart rx task", 4*1024, NULL, 2, NULL);
}


static void uart_rx_task( void *pvParameter )
{
  uint8_t rx_byte;
  static rx_data_state_e rx_state = RX_DATA_STATE_START;

  // esp_task_wdt_add(NULL);  // Register this task with watchdog

  while (1)
  {
    // Optional: Reset watchdog manually if enabled
    // esp_task_wdt_reset();
    int len = uart_read_bytes(UART_NUM, &rx_byte, 1, pdMS_TO_TICKS(100));
    if (len > 0)
    {
      ESP_LOGI( TAG, "Data Received");
      switch ( rx_state )
      {
        case RX_DATA_STATE_START:
          if (rx_byte == PACKET_START )
          {
            rx_state = RX_DATA_STATE_COPY_DATA;
            rx_buff_idx = 0;
          }
          break;
        case RX_DATA_STATE_COPY_DATA:
          if( rx_byte == PACKET_END )
          {
            rx_state = RX_DATA_STATE_END;
          }
          else
          {
            // copy data
            if ( rx_buff_idx < RX_BUFF_SIZE - 1 )
            {
              rx_buff[rx_buff_idx++] = rx_byte;
            }
          }
          break;
        case RX_DATA_STATE_END:
          rx_buff[rx_buff_idx] = '\0';
          ESP_LOGI( TAG, "%s", rx_buff );
          rx_state = RX_DATA_STATE_START;
          break;
      }
    }
    else
    {
      // No data received, yield to other tasks
      vTaskDelay(pdMS_TO_TICKS(500));
      ESP_LOGI( TAG, "Data Not Received");
    }
  }
}
