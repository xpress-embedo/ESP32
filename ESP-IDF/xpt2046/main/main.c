#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TOUCH_SPI_CLK_SPEED           (250 * 1000)
#define TOUCH_SPI_HOST                (SPI3_HOST)
#define TOUCH_SPI_MOSI                (GPIO_NUM_23)
#define TOUCH_SPI_MISO                (GPIO_NUM_19)
#define TOUCH_SPI_SCLK                (GPIO_NUM_18)
#define TOUCH_SPI_CS                  (GPIO_NUM_2)
#define TFT_SPI_CS                    (GPIO_NUM_5)

#define TOUCH_CS_LOW()                gpio_set_level(TOUCH_SPI_CS, 0)
#define TOUCH_CS_HIGH()               gpio_set_level(TOUCH_SPI_CS, 1)
#define TFT_CS_LOW()                  gpio_set_level(TFT_SPI_CS, 0)
#define TFT_CS_HIGH()                 gpio_set_level(TFT_SPI_CS, 1)

// Private Variables
spi_device_handle_t spi_touch_handle;

// Private Functions
void touch_read_data( uint8_t cmd, uint8_t *data, uint8_t len );

void app_main(void)
{
  //Initialize non-SPI GPIOs
  gpio_config_t io_conf = {};
  io_conf.pin_bit_mask = (1u<<TOUCH_SPI_CS) | (1u<<TFT_SPI_CS);
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pull_up_en = true;
  gpio_config(&io_conf);

  TFT_CS_HIGH();
  TFT_CS_LOW();
  TFT_CS_HIGH();

  TOUCH_CS_HIGH();
  TOUCH_CS_LOW();
  TOUCH_CS_HIGH();

  // bus configuration
  spi_bus_config_t touch_bus_cfg =
  {
    .mosi_io_num = TOUCH_SPI_MOSI,
    .miso_io_num = TOUCH_SPI_MISO,
    .sclk_io_num = TOUCH_SPI_SCLK,
    .max_transfer_sz = 200,
    .quadhd_io_num = -1,
    .quadwp_io_num = -1,
  };

  // touch device configuration
  spi_device_interface_config_t touch_dev_config =
  {
    .clock_speed_hz = TOUCH_SPI_CLK_SPEED,
    .mode = 0,
    .spics_io_num = -1,
    .input_delay_ns = 0,
    .queue_size = 50,
    .pre_cb = NULL,
    .post_cb = NULL,
    .flags = SPI_DEVICE_NO_DUMMY,
  };

  esp_err_t ret;
  spi_dma_chan_t dma_channel = SPI_DMA_DISABLED;   // don't enable DMA on Channel-0

  // initialize the SPI bus
  ret = spi_bus_initialize(TOUCH_SPI_HOST, &touch_bus_cfg, dma_channel);
  assert(ret == ESP_OK);

  // attach the touch to the SPI bus
  ret = spi_bus_add_device(TOUCH_SPI_HOST, &touch_dev_config, &spi_touch_handle);
  assert(ret == ESP_OK);

  TOUCH_CS_LOW();
  TOUCH_CS_HIGH();

  uint8_t data1[2] = {0x00, 0x00};
  uint8_t data2[2] = {0x00, 0x00};

  while (true)
  {
    touch_read_data(0x90, data1, 2);
    printf("X = 0x%x, 0x%X\n", data1[0], data1[1] );

    touch_read_data(0xD0, data2, 2);
    printf("X = 0x%x, 0x%X\n\n", data2[0], data2[1] );
    sleep(2);
  }
}

#if 0
void touch_read_data( uint8_t cmd, uint8_t *data, uint8_t len )
{
  esp_err_t ret;
  spi_transaction_t t;

  TOUCH_CS_LOW();
  memset( &t, 0x00, sizeof(t) );    // zero out the transaction
  t.length = (len + sizeof(cmd)) * 8;
  t.rxlength = (len * 8);
  t.cmd = cmd;
  t.rx_buffer = data;
  t.flags = SPI_TRANS_USE_RXDATA;
  ret = spi_device_polling_transmit(spi_touch_handle, &t);  // transmit
  // ret = spi_device_transmit(spi_touch_handle, &t);  // transmit
  printf("Function Data = 0x%x, 0x%X\n\n", t.rx_data[0], t.rx_data[1]);
  TOUCH_CS_HIGH();
  assert(ret == ESP_OK);
}
#else
void touch_read_data( uint8_t cmd, uint8_t *data, uint8_t len )
{
  esp_err_t ret;
  spi_transaction_t t;
  TOUCH_CS_LOW();
  memset( &t, 0x00, sizeof(t) );    // zero out the transaction
  t.length = 8;                     // Commands are 8-bits
  t.tx_buffer = &cmd;               // The data is command itself
  ret = spi_device_polling_transmit(spi_touch_handle, &t);  // transmit
  assert(ret == ESP_OK);            // should have no issues
  if( len )
  {
    memset( &t, 0x00, sizeof(t) );    // zero out the transaction
    t.length = len*8;
    t.rxlength = (len * 8);
    t.rx_buffer = data;
    t.flags = SPI_TRANS_USE_RXDATA;
    ret = spi_device_polling_transmit(spi_touch_handle, &t);  // transmit
    printf("Function Data = 0x%x, 0x%X\n\n", t.rx_data[0], t.rx_data[1]);
    TOUCH_CS_HIGH();
    assert(ret == ESP_OK);
  }
  TOUCH_CS_HIGH();
}
#endif
