/*
 * display_mng.c
 *
 *  Created on: Dec 15, 2023
 *      Author: xpress_embedo
 */

#include "esp_heap_caps.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ili9341.h"
#include "display_mng.h"

// Private Macros
#define DISP_SPI_CLK_SPEED            (40*1000*1000)

#define DISP_BUFFER_SIZE              (DISP_HOR_RES_MAX * 40)

// Private Variables
spi_device_handle_t spi_disp_handle;
// ---------------------------LVGL Related Stuff-------------------------------

// Private Function Prototypes
static void display_driver_init( void );
static void display_pre_tx_cb( spi_transaction_t *t );


// Public Function Definitions
void display_init( void )
{
  display_driver_init();
  ili9341_init();
  ili9341_set_orientation(LCD_LANDSCAPE);
  ili9341_set_orientation(LCD_ORIENTATION_180);
  ili9341_set_orientation(LCD_PORTRAIT);
  // Some Test Code Starts
  ili9341_draw_pixel( 0u, 0u, ILI9341_RED );
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ili9341_set_orientation( LCD_ORIENTATION_0 );
  ili9341_fill( ILI9341_RED );
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ili9341_set_orientation( LCD_ORIENTATION_90 );
  ili9341_fill( ILI9341_ORANGE );
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ili9341_set_orientation( LCD_ORIENTATION_180 );
  ili9341_fill( ILI9341_PINK );
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ili9341_set_orientation( LCD_ORIENTATION_270 );
  ili9341_fill( ILI9341_DARKGREEN );
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ili9341_fill( ILI9341_DARKCYAN );
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ili9341_fill_rectangle( 0u, 0u, 50u, 50u, ILI9341_WHITE );
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ili9341_fill_rectangle( 50u, 50u, 100u, 100u, ILI9341_ORANGE );
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ili9341_fill_rectangle( 100u, 100u, 150u, 150u, ILI9341_DARKGREEN );
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ili9341_draw_circle( 20, 20, 20, ILI9341_PINK);
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ili9341_draw_line( 0u, 0u, ili9341_get_width(), ili9341_get_height(), ILI9341_DARKGREEN);
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ili9341_draw_h_line( 10u, 10u, 50u, ILI9341_ORANGE );
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ili9341_draw_v_line( 10u, 10u, 50u, ILI9341_ORANGE );
  vTaskDelay(500 / portTICK_PERIOD_MS);
  // Some Test Code Ends
}

void display_mng( void )
{

}

/*
 * Send Command to the LCD. Uses the "spi_device_polling_transmit", which waits
 * unit the transfer is complete.
 * Since command transactions are usually small, they are handled in polling
 * mode for higher speed. The overhead of interrupt transaction is more than
 * just waiting for the transaction to complete.
 */
void display_send_cmd( uint8_t cmd, const uint8_t *data, int len )
{
  esp_err_t ret;
  spi_transaction_t t;

  DISP_CS_LOW();
  DISP_DC_LOW();
  // Send Command
  memset( &t, 0x00, sizeof(t) );    // zero out the transaction
  t.length = 8;                     // Commands are 8-bits
  t.tx_buffer = &cmd;               // The data is command itself
  ret = spi_device_polling_transmit(spi_disp_handle, &t); // transmit
  assert(ret == ESP_OK);            // should have no issues
  DISP_DC_HIGH();
  // if there is some data with command
  if( len )
  {
    memset( &t, 0x00, sizeof(t) );    // zero out the transaction
    t.length = len*8;                 // length is in bytes while transaction length is in bits
    t.tx_buffer = data;               // The data is command itself
    ret = spi_device_polling_transmit(spi_disp_handle, &t); // transmit
    DISP_CS_HIGH();
    assert(ret == ESP_OK);            // should have no issues
  }
  DISP_CS_HIGH();
}

/*
 * Send Data to the LCD. Uses the "spi_device_polling_transmit", which waits
 * unit the transfer is complete.
 * Since data transactions are usually small, they are handled in polling
 * mode for higher speed. The overhead of interrupt transaction is more than
 * just waiting for the transaction to complete.
 */
void display_send_data( const uint8_t *data, int len )
{
  esp_err_t ret;
  spi_transaction_t t;
  if( len == 0 )
    return;                         // no need to send anything

  DISP_CS_LOW();
  DISP_DC_HIGH();
  memset( &t, 0x00, sizeof(t) );    // zero out the transaction
  t.length = len*8;                 // length is in bytes while transaction length is in bits
  t.tx_buffer = data;               // Data
  // Not used any more
  // t.user = (void*)1;                // transaction id, keep it 1 for data mode
  ret = spi_device_polling_transmit(spi_disp_handle, &t); // transmit
  DISP_CS_HIGH();
  assert(ret == ESP_OK);            // should have no issues
}

// Private Function Definitions
static void display_driver_init( void )
{
  esp_err_t ret;
  spi_dma_chan_t dma_channel = SPI_DMA_CH1;   // don't enable DMA on Channel-0

  // SPI bus configuration
  spi_bus_config_t bus_cfg =
  {
    .mosi_io_num = DISP_SPI_MOSI,
    .miso_io_num = DISP_SPI_MISO,
    .sclk_io_num = DISP_SPI_SCLK,
    .max_transfer_sz = DISP_BUFFER_SIZE,  // maximum transfer size in bytes
    .quadhd_io_num = -1,
    .quadwp_io_num = -1,
  };

  // LCD Device configuration
  spi_device_interface_config_t dev_config =
  {
    .clock_speed_hz = DISP_SPI_CLK_SPEED,
    .mode = 0,                      // SPI mode, representing pair of CPOL, CPHA
    // .spics_io_num = DISP_SPI_CS,    // chip select for this device
    .spics_io_num = -1,             // chip select for this device (manual control)
    .input_delay_ns = 0,            // todo
    .queue_size = 50,               // Transaction queue size. This sets how
                                    // many transactions can be 'in the air'
                                    // (queued using spi_device_queue_trans
                                    // but not yet finished using
                                    // spi_device_get_trans_result) at the same time
    // .pre_cb = display_pre_tx_cb,    // callback to be called before transmission is started
    .pre_cb = NULL,                 // callback to be called before transmission is started
    .post_cb = NULL,                // callback to be called after transmission is completed
    .flags = SPI_DEVICE_NO_DUMMY,
  };

  // initialize the SPI bus
  ret = spi_bus_initialize(DISP_SPI_HOST, &bus_cfg, dma_channel);
  assert(ret == ESP_OK);

  // attach the LCD to the SPI bus
  ret = spi_bus_add_device(DISP_SPI_HOST, &dev_config, &spi_disp_handle);
  assert(ret == ESP_OK);

  //Initialize non-SPI GPIOs
  gpio_config_t io_conf = {};
  io_conf.pin_bit_mask = (1u<<DISP_PIN_DC) | (1u<<DISP_SPI_CS);
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pull_up_en = true;
  gpio_config(&io_conf);

  DISP_CS_LOW();
  DISP_CS_HIGH();
}

/*
 * This function is called (in IRQ context) just before a transmission starts.
 * It will set the D/C line to the value indicated in the user field.
 */
static void display_pre_tx_cb( spi_transaction_t *t )
{
  int dc = (int)t->user;
  gpio_set_level(DISP_PIN_DC, dc);
}
