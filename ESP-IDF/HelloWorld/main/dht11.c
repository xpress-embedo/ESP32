/*
 * dht11.c
 *
 *  Created on: 22-Aug-2023
 *      Author: xpress_embedo
 */

#include "esp_timer.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "dht11.h"

/* Macros */
#define DHT11_INITIAL_WAKEUP_DELAY          (1000)      /* Initial wait time in milli seconds */
#define DHT11_READING_WAIT_TIME             (2000000)   /* Wait time between two consecutive readings in micro seconds*/

/* Private Variables */
static gpio_num_t dht_gpio;
static int64_t last_read_time = -2000000;
static dht11_reading_t last_read;

/* Private Function Prototypes */
static int dht11_wait_or_timeout(uint16_t useconds, int level);
static int dht11_check_CRC(uint8_t data[]);
static void dht11_send_start_signal( void );
static int dht11_check_response( void );
static dht11_reading_t dht11_timeout_error( void );
static dht11_reading_t dht11_crc_error( void );


/* Public Function Definitions */
void dht11_init(gpio_num_t gpio_num)
{
  /* Wait for some seconds to make the device pass its initial unstable status */
  vTaskDelay(DHT11_INITIAL_WAKEUP_DELAY / portTICK_PERIOD_MS);
  dht_gpio = gpio_num;
}

dht11_reading_t dht11_read( void )
{
  /* DHT11 sensor can take up-to 2 seconds for updated values, hence if some one
   * call this function too early, then we should return the old value
   * */
  if( (esp_timer_get_time() - DHT11_READING_WAIT_TIME) < last_read_time)
  {
    return last_read;
  }

  last_read_time = esp_timer_get_time();

  uint8_t data[5] = {0,0,0,0,0};

  dht11_send_start_signal();

  if( dht11_check_response() == DHT11_TIMEOUT_ERROR )
  {
    return last_read = dht11_timeout_error();
  }

  /* Read response */
  for(int i = 0; i < 40; i++)
  {
    /* Initial data */
    if(dht11_wait_or_timeout(50, 0) == DHT11_TIMEOUT_ERROR)
    {
      return last_read = dht11_timeout_error();
    }

    if(dht11_wait_or_timeout(70, 1) > 28)
    {
      /* Bit received was a 1 */
      data[i/8] |= (1 << (7-(i%8)));
    }
  }

  if(dht11_check_CRC(data) != DHT11_CRC_ERROR)
  {
    last_read.status = DHT11_OK;
    last_read.temperature = data[2];
    last_read.humidity = data[0];
    return last_read;
  }
  else
  {
    return last_read = dht11_crc_error();
  }
}

/* Private Function Definitions */
static int dht11_wait_or_timeout(uint16_t microSeconds, int level)
{
  int micros_ticks = 0;
  while(gpio_get_level(dht_gpio) == level)
  {
    if(micros_ticks++ > microSeconds)
    {
      return DHT11_TIMEOUT_ERROR;
    }
    ets_delay_us(1);
  }
  return micros_ticks;
}

static int dht11_check_CRC(uint8_t data[])
{
  dht11_status_e dht_status = DHT11_CRC_ERROR;
  if(data[4] == (data[0] + data[1] + data[2] + data[3]))
  {
    dht_status = DHT11_OK;
  }
  return (int)dht_status;
}

static void dht11_send_start_signal()
{
  gpio_set_direction(dht_gpio, GPIO_MODE_OUTPUT);
  gpio_set_level(dht_gpio, 0);
  ets_delay_us(20 * 1000);
  gpio_set_level(dht_gpio, 1);
  ets_delay_us(40);
  gpio_set_direction(dht_gpio, GPIO_MODE_INPUT);
}

static int dht11_check_response( void )
{
  /* Wait for next step ~80us*/
  if(dht11_wait_or_timeout(80, 0) == DHT11_TIMEOUT_ERROR)
    return DHT11_TIMEOUT_ERROR;

  /* Wait for next step ~80us*/
  if(dht11_wait_or_timeout(80, 1) == DHT11_TIMEOUT_ERROR)
    return DHT11_TIMEOUT_ERROR;

  return DHT11_OK;
}

static dht11_reading_t dht11_timeout_error()
{
  dht11_reading_t timeout_error = {DHT11_TIMEOUT_ERROR, -1, -1};
  return timeout_error;
}

static dht11_reading_t dht11_crc_error()
{
  dht11_reading_t crc_error = {DHT11_CRC_ERROR, -1, -1};
  return crc_error;
}
