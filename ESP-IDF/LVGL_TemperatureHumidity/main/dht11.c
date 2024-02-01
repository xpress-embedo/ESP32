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
#define DHT11_INITIAL_WAKEUP_DELAY          (1000)          /* Initial wait time in milli seconds */
#define DHT11_READING_WAIT_DELAY            (2*1000*1000)   /* Wait time between two consecutive readings in micro seconds*/

#define DHT11_START_SIGNAL_PULL_DOWN_DELAY  (20*1000)       /* 20ms time */
#define DHT11_START_SIGNAL_PULL_UP_DELAY    (40)            /* 40us time */

/* Private Variables */
static gpio_num_t dht_gpio;
static int64_t last_read_time = -2000000;
static dht11_reading_t last_read;

/* Private Function Prototypes */
static int dht11_wait_or_timeout(uint16_t useconds, int level);
static int dht11_check_checksum(uint8_t data[]);
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
  if( (esp_timer_get_time() - DHT11_READING_WAIT_DELAY) < last_read_time)
  {
    return last_read;
  }

  last_read_time = esp_timer_get_time();

  uint8_t data[5] = {0,0,0,0,0};

  /* trigger the start signal */
  dht11_send_start_signal();

  /* Check for DHT11 Host Signal, here DHT11 should pull the line low for 80us
   * and then pull the line high for 80 us
   */
  if( dht11_check_response() == DHT11_TIMEOUT_ERROR )
  {
    return last_read = dht11_timeout_error();
  }

  /* Now DHT11 will send 40 bits of data, each bit transmission begins with the
   * low-voltage level that last around 50us, the following high-voltage level
   * signal length decides whether the bit is "1" or "0"
   */
  for(int i = 0; i < 40; i++)
  {
    /* Initial low signal, should be around 50 us, if more than that it is a
    timeout error. */
    if(dht11_wait_or_timeout(50, 0) == DHT11_TIMEOUT_ERROR)
    {
      return last_read = dht11_timeout_error();
    }

    /* Now the high signal length will determine whether it is a bit 1 or 0, here
    we check maximum for 70 us second, but if pin gets low and the micro ticks
    are greater than 28 us seconds, then it means bit is 1 else bit is 0 */
    if(dht11_wait_or_timeout(70, 1) > 28)
    {
      /* Bit received is a 1 */
      data[i/8] |= (1 << (7-(i%8)));
    }
  }

  /* last step is to validate the received data by checking the checksum */
  if(dht11_check_checksum(data) != DHT11_CHECKSUM_ERROR)
  {
    last_read.status = DHT11_OK;
    last_read.temperature = data[2];
    last_read.humidity = data[0];
  }
  else
  {
    last_read = dht11_crc_error();
  }

  return last_read;
}

/* Private Function Definitions */
static int dht11_wait_or_timeout(uint16_t useconds, int level)
{
  int micros_ticks = 0;
  while(gpio_get_level(dht_gpio) == level)
  {
    if(micros_ticks++ > useconds)
    {
      return DHT11_TIMEOUT_ERROR;
    }
    ets_delay_us(1);
  }
  return micros_ticks;
}

static int dht11_check_checksum( uint8_t data[] )
{
  dht11_status_e dht_status = DHT11_CHECKSUM_ERROR;
  if(data[4] == (data[0] + data[1] + data[2] + data[3]))
  {
    dht_status = DHT11_OK;
  }
  return (int)dht_status;
}

static void dht11_send_start_signal( void )
{
  /* Request Stage:
   * To make the DHT11 send the sensor readings we have to send a request.
   * The Request is to pull down the bus for more than 18ms, in order to give
   * DHT11 time to understand it and then pull it up for 40 micro-seconds
   */
  gpio_config_t io_conf = {};
  io_conf.pin_bit_mask = (1u<<dht_gpio);
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pull_up_en = true;
  gpio_config(&io_conf);
  // NOTE: I have to add the above code, without this GPIO is not working
  // so I added above configuration code and commented the below one
  // gpio_set_direction(dht_gpio, GPIO_MODE_OUTPUT);
  gpio_set_level(dht_gpio, 0);
  ets_delay_us(DHT11_START_SIGNAL_PULL_DOWN_DELAY);   // 20ms delay
  gpio_set_level(dht_gpio, 1);
  ets_delay_us(DHT11_START_SIGNAL_PULL_UP_DELAY);     // 40us delay
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
  dht11_reading_t timeout_error = { DHT11_TIMEOUT_ERROR, -1, -1 };
  return timeout_error;
}

static dht11_reading_t dht11_crc_error()
{
  dht11_reading_t crc_error = { DHT11_CHECKSUM_ERROR, -1, -1 };
  return crc_error;
}
