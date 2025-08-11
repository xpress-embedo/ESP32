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

#include "main.h"
#include "wifi_app.h"
#include "gui_mng.h"
#include "mqtt_app.h"

// Private Macros
#define MAIN_TASK_PERIOD                    (1000)
#define UART_PORT_NUM                       UART_NUM_0
#define UART_RX_BUF_SIZE                    128
#define TRAFFIC_PAYLOAD_LEN                 24  // "0:G10,1:R13,2:R26,3:R39"

// Private Variables
static const char *TAG = "APP";

// Private Function Declarations
static void uart_start( void );
static void uart_task(void *pvParameter);
static void parse_traffic_payload(const char *payload, traffic_light_t *lights);

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

  while (true )
  {
    // Wait before next
    vTaskDelay(MAIN_TASK_PERIOD / portTICK_PERIOD_MS);
  }
}

// Private Function Definitions
static void uart_start( void )
{
  const uart_config_t uart_config =
  {
      .baud_rate = 115200,
      .data_bits = UART_DATA_8_BITS,
      .parity    = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
  };
  uart_driver_install( UART_PORT_NUM, UART_RX_BUF_SIZE * 2, 0, 0, NULL, 0 );
  uart_param_config( UART_PORT_NUM, &uart_config );
  uart_set_pin( UART_PORT_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE );

  xTaskCreate(&uart_task, "uart task", 1024, NULL, 5, NULL);
}

#define FRAME_START         '<'
#define FRAME_END           '>'
#define FRAME_MAX_LEN       64

static void uart_task(void *pvParameter)
{
  uint8_t rx_buf[TRAFFIC_PAYLOAD_LEN + 1];  // +1 for null terminator
  traffic_light_t lights[TRAFFIC_LIGHT_SIDES];

  memset(lights, 0, sizeof(lights));
  while (1)
  {
    int len = uart_read_bytes(UART_PORT_NUM, rx_buf, TRAFFIC_PAYLOAD_LEN, pdMS_TO_TICKS(1000));

    if (len == TRAFFIC_PAYLOAD_LEN)
    {
      rx_buf[len] = '\0';  // Null-terminate

      // Validate framing
      if (rx_buf[0] == '<' && rx_buf[len - 1] == '>')
      {
        parse_traffic_payload((char *)rx_buf, lights);

        // Optional: print parsed values
        /*
        for (int i = 0; i < TRAFFIC_LIGHT_SIDES; i++)
        {
          printf("Side %d: %c G=%d Y=%d R=%d\n", i, lights[i].color,
                  lights[i].green_time, lights[i].yellow_time, lights[i].red_time);
        }
        */
        // if mqtt is not working then only serial data should be considered
        if ( mqtt_get_connection_status() == false )
        {
          gui_send_event( GUI_MNG_EV_TRAFFIC_CTRL_V2, (uint8_t*)(&lights) );
        }
      }
    }
  }
}

static void parse_traffic_payload(const char *payload, traffic_light_t *lights)
{
  // This is packet format
  // <0:G10,1:R13,2:R26,3:R39>
  #define SEGMENT_LEN         (6u)
  const char *p = payload + 1;  // Skip initial '<'
  uint8_t i;

  for ( i = 0; i < TRAFFIC_LIGHT_SIDES; i++ )
  {
    int index;
    char color;
    int duration;
    sscanf(p + i * SEGMENT_LEN, "%d:%c%2d", &index, &color, &duration);

    lights[index].color = color;
    lights[index].green_time = 0;
    lights[index].yellow_time = 0;
    lights[index].red_time = 0;

    switch ( color )
    {
      case 'G':
        lights[index].green_time = duration;
        break;
      case 'Y':
        lights[index].yellow_time = duration;
        break;
      case 'R':
        lights[index].red_time = duration;
        break;
      default:
        break;
    }
  }
}

