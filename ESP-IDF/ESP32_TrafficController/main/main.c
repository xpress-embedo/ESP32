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
#include "esp_task_wdt.h"

#include "main.h"
#include "wifi_app.h"
#include "gui_mng.h"
#include "mqtt_app.h"

// Private Macros
#define MAIN_TASK_PERIOD                    (1000)
// GPIO Connections for LEDs
#define TRAFFIC_LED_1_RED                   GPIO_NUM_16
#define TRAFFIC_LED_1_YELLOW                GPIO_NUM_17
#define TRAFFIC_LED_1_GREEN                 GPIO_NUM_12   // there could be some issue with this pin
#define TRAFFIC_LED_2_GREEN                 GPIO_NUM_13
#define TRAFFIC_LED_2_YELLOW                GPIO_NUM_15
#define TRAFFIC_LED_2_RED                   GPIO_NUM_4
#define TRAFFIC_LED_3_GREEN                 GPIO_NUM_25
#define TRAFFIC_LED_3_YELLOW                GPIO_NUM_33
#define TRAFFIC_LED_3_RED                   GPIO_NUM_32
#define TRAFFIC_LED_4_RED                   GPIO_NUM_14
#define TRAFFIC_LED_4_YELLOW                GPIO_NUM_27
#define TRAFFIC_LED_4_GREEN                 GPIO_NUM_26

// GPIO22 is TXD and GPIO21 as RXD
#define UART_NUM                            UART_NUM_1
#define TXD_PIN                             GPIO_NUM_21
#define RXD_PIN                             GPIO_NUM_22
#define RX_BUFF_SIZE                        (100u)
// Header and Footer for Serial Packet
#define PACKET_START                        '<'
#define PACKET_END                          '>'

// Private Variables
static const char *TAG = "APP";
static traffic_light_t traffic_light_time[TRAFFIC_LIGHT_SIDES] = { 0 };
static bool serial_connect_state = false;

// global variables
const uint8_t TAB_GREEN_LIGHT[TRAFFIC_LIGHT_SIDES] =
{
    TRAFFIC_LED_1_GREEN,
    TRAFFIC_LED_2_GREEN,
    TRAFFIC_LED_3_GREEN,
    TRAFFIC_LED_4_GREEN,
};

const uint8_t TAB_YELLOW_LIGHT[TRAFFIC_LIGHT_SIDES] =
{
    TRAFFIC_LED_1_YELLOW,
    TRAFFIC_LED_2_YELLOW,
    TRAFFIC_LED_3_YELLOW,
    TRAFFIC_LED_4_YELLOW,
};

const uint8_t TAB_RED_LIGHT[TRAFFIC_LIGHT_SIDES] =
{
    TRAFFIC_LED_1_RED,
    TRAFFIC_LED_2_RED,
    TRAFFIC_LED_3_RED,
    TRAFFIC_LED_4_RED,
};

// Private Function Declarations
static void serial_init( void );
static void serial_start( void );
static void serial_rx_task(void *pvParameter);
static void serial_parse_traffic_payload(const char *payload, traffic_light_t *lights);
static void serial_set_connect_state( bool state );
static bool serial_is_connected( void );
static void traffic_lights_init( void );

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
  serial_start();

  // initialize all traffic leds
  traffic_lights_init();

  const char *data = "Traffic Controller Starting\r\n";
  uart_write_bytes(UART_NUM, data, strlen(data));

  while (true )
  {
    // Wait before next
    vTaskDelay(MAIN_TASK_PERIOD / portTICK_PERIOD_MS);
  }
}

void traffic_lights_on( gpio_num_t gpio_num )
{
  gpio_set_level(gpio_num, 1);
}

void traffic_lights_off( gpio_num_t gpio_num )
{
  gpio_set_level(gpio_num, 0);
}

// Private Function Definitions
static void serial_init( void )
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

static void serial_start( void )
{
  serial_init();
  xTaskCreate(&serial_rx_task, "uart rx task", 4*1024, NULL, 2, NULL);
}


static void serial_rx_task( void *pvParameter )
{
  uint8_t byte;
  static char rx_buff[RX_BUFF_SIZE];
  static uint8_t rx_idx = 0;
  static bool receiving = false;

  esp_task_wdt_add(NULL);

  while (true)
  {
    esp_task_wdt_reset();

    int len = uart_read_bytes(UART_NUM, &byte, 1, pdMS_TO_TICKS(100));
    if (len > 0)
    {
      if ( byte == PACKET_START )
      {
        receiving = true;
        rx_idx = 0;
      }
      else if (byte == PACKET_END && receiving)
      {
        receiving = false;
        rx_buff[rx_idx] = '\0';
        // ESP_LOGI("UART_RX", "Received: %s", rx_buff);

        // MQTT is not connected, means we have to switch to Serial Mode
        if( mqtt_get_connection_status() == false )
        {
          // Parse and update traffic lights
          serial_parse_traffic_payload(rx_buff, traffic_light_time);

          // check if system knows about serial connection
          if( serial_is_connected() == false )
          {
            // set the state to true, indicating that we are using serial data instead of MQTT data
            serial_set_connect_state(true);
            ESP_LOGI( TAG, "Serial Connection is Selected");
            // also send an event to gui manager
            gui_send_event( GUI_MNG_EV_SERIAL_CONNECTED , NULL );
          }
          // NOTE: when I send the event GUI_MNG_EV_TRAFFIC_CTRL_V2 directly
          // after the GUI_MNG_EV_SERIAL_CONNECTED, my ESP32 gets re-started
          // that's why I moved this to else part, but should work, this is the
          // purpose of queue: later I tested and seems to be working fine
          else
          {
            // send event to update screens
            // gui_send_event( GUI_MNG_EV_TRAFFIC_CTRL_V2,  (uint8_t*)(&traffic_light_time) );
          }
          gui_send_event( GUI_MNG_EV_TRAFFIC_CTRL_V2,  (uint8_t*)(&traffic_light_time) );
        }
        else
        {
          if( serial_is_connected() == true )
          {
            serial_set_connect_state(false);
            ESP_LOGI( TAG, "Serial Connection De-selected, MQTT has taken over");
          }
        }
      }
      else if (receiving && rx_idx < RX_BUFF_SIZE - 1)
      {
        rx_buff[rx_idx++] = byte;
      }
    }
  }
}

static void serial_parse_traffic_payload(const char *payload, traffic_light_t *lights)
{
  // NOTE: there is really a small difference between this function and mqtt_parse_traffic_payload
  // and both can be merged, but for now, let's keep them like this only
  // This is packet format
  // 0:G10,1:R13,2:R26,3:R39
  #define SEGMENT_LEN         (6u)
  const char *p = payload;        // here the packet is already with '<' and '>' in comparison to mqtt_parse_traffic_payload function
  uint8_t i;

  for ( i = 0; i < TRAFFIC_LIGHT_SIDES; i++ )
  {
    int index;
    char color;
    int duration;
    sscanf(p + i * SEGMENT_LEN, "%d:%c%2d", &index, &color, &duration);
    // printf("Index = %d, Color = %c, Duration = %d\r\n", index, color, duration );
    lights[index].color = color;
    // should be already zero due to memset function, but done again to be safer
    lights[index].green_time = 0;
    lights[index].yellow_time = 0;
    lights[index].red_time = 0;

    // turn off the leds and then turn them on individually
    traffic_lights_off( TAB_GREEN_LIGHT[index] );
    traffic_lights_off( TAB_YELLOW_LIGHT[index] );
    traffic_lights_off( TAB_RED_LIGHT[index] );

    switch ( color )
    {
      case 'G':
        lights[index].green_time = duration;
        traffic_lights_on( TAB_GREEN_LIGHT[index] );
        break;
      case 'Y':
        lights[index].yellow_time = duration;
        traffic_lights_on( TAB_YELLOW_LIGHT[index] );
        break;
      case 'R':
        lights[index].red_time = duration;
        traffic_lights_on( TAB_RED_LIGHT[index] );
        break;
      default:
        break;
    }
    // for debugging
    // ESP_LOGI( TAG, "idx=%d, G=%d, Y=%d, R=%d", i, lights[i].green_time, lights[i].yellow_time, lights[i].red_time );
  }
}

static void serial_set_connect_state( bool state )
{
  serial_connect_state = state;
}

static bool serial_is_connected( void )
{
  return serial_connect_state;
}


static void init_gpio_output( gpio_num_t gpio )
{
  gpio_config_t io_conf =
  {
      .pin_bit_mask = (1ULL << gpio),
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE
  };
  gpio_config(&io_conf);

}

static void traffic_lights_init( void )
{
  for (uint8_t i=0; i < TRAFFIC_LIGHT_SIDES; i++ )
  {
    init_gpio_output( TAB_GREEN_LIGHT[i] );
    init_gpio_output( TAB_YELLOW_LIGHT[i] );
    init_gpio_output( TAB_RED_LIGHT[i] );
  }
}
