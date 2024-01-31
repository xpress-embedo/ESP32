/*
 * tft.c
 *
 *  Created on: Dec 15, 2023
 *      Author: xpress_embedo
 */

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tft.h"

// Private Macros
#define TFT_SPI_CLK_SPEED             (40*1000*1000)      // 40MHz
#define TOUCH_SPI_CLK_SPEED           (25*1000*100)       // 2.5MHz

// User Macros
#define SPI_USER_FLAG_DC_LOW          (0x01)              // for command
#define SPI_USER_FLAG_DC_HIGH         (0x02)              // for data
#define SPI_USER_FLAG_FLUSH_READY     (0x03)              // lvgl flush ready

// Private Variables
spi_device_handle_t spi_tft_handle;
spi_device_handle_t spi_touch_handle;
// ---------------------------LVGL Related Stuff-------------------------------

// Private Function Prototypes
static void tft_driver_init( void );
// NOTE: this function was used to control D/C line using auto callback
static void tft_pre_tx_cb( spi_transaction_t *t );
static void tft_post_tx_cb( spi_transaction_t *t );

// Public Function Definitions

/**
 * @brief Initialize the TFT
 * @param  
 */
void tft_init( void )
{
  tft_driver_init();
  ili9341_init();
  ili9341_set_orientation(LCD_ORIENTATION_270);
//  ili9341_set_orientation(LCD_LANDSCAPE);
//  ili9341_set_orientation(LCD_ORIENTATION_180);
//  ili9341_set_orientation(LCD_PORTRAIT);
//  // Some Test Code Starts
//  ili9341_draw_pixel( 0u, 0u, ILI9341_RED );
//  tft_delay_ms(500);
//  ili9341_set_orientation( LCD_ORIENTATION_0 );
//  ili9341_fill( ILI9341_RED );
//  tft_delay_ms(500);
//  ili9341_set_orientation( LCD_ORIENTATION_90 );
//  ili9341_fill( ILI9341_ORANGE );
//  tft_delay_ms(500);
//  ili9341_set_orientation( LCD_ORIENTATION_180 );
//  ili9341_fill( ILI9341_PINK );
//  tft_delay_ms(500);
//  ili9341_set_orientation( LCD_ORIENTATION_270 );
//  ili9341_fill( ILI9341_DARKGREEN );
//  tft_delay_ms(500);
//  ili9341_fill( ILI9341_DARKCYAN );
//  tft_delay_ms(500);
//  ili9341_fill_rectangle( 0u, 0u, 50u, 50u, ILI9341_WHITE );
//  tft_delay_ms(500);
//  ili9341_fill_rectangle( 50u, 50u, 100u, 100u, ILI9341_ORANGE );
//  tft_delay_ms(500);
//  ili9341_fill_rectangle( 100u, 100u, 150u, 150u, ILI9341_DARKGREEN );
//  tft_delay_ms(500);
//  ili9341_draw_circle( 20, 20, 20, ILI9341_PINK);
//  tft_delay_ms(500);
//  ili9341_draw_line( 0u, 0u, ili9341_get_width(), ili9341_get_height(), ILI9341_DARKGREEN);
//  tft_delay_ms(500);
//  ili9341_draw_h_line( 10u, 10u, 50u, ILI9341_ORANGE );
//  tft_delay_ms(500);
//  ili9341_draw_v_line( 10u, 10u, 50u, ILI9341_ORANGE );
//  tft_delay_ms(500);
  // Some Test Code Ends
}

/**
 * @brief Delay in milliseconds
 *        This function uses FreeRTOS task delay API's
 * @param delay value in milliseconds
 */
void tft_delay_ms(uint32_t delay)
{
  vTaskDelay(delay / portTICK_PERIOD_MS);
}

/**
 * @brief Send Command/Data to the TFT Controller
 *        Send Command to the LCD. Uses the "spi_device_polling_transmit", which
 *        waits until the transfer is complete.
 *        Since command transactions are usually small, they are handled in 
 *        polling mode for higher speed. The overhead of interrupt transaction 
 *        is more than just waiting for the transaction to complete.
 * @param cmd   command value
 * @param data  data buffer pointer
 * @param len   length of the data
 */
void tft_send_cmd( uint8_t cmd, const uint8_t *data, size_t len )
{
  esp_err_t ret;
  spi_transaction_t t;

  TFT_CS_LOW();
  TFT_DC_LOW();
  // Send Command
  memset( &t, 0x00, sizeof(t) );          // zero out the transaction
  t.length = 8;                           // Commands are 8-bits
  t.tx_buffer = &cmd;                     // The data is command itself
  t.user = (void*)SPI_USER_FLAG_DC_LOW;   // transaction id, keep it 0 for command mode
  ret = spi_device_polling_transmit(spi_tft_handle, &t);  // transmit
  assert(ret == ESP_OK);                  // should have no issues
  TFT_DC_HIGH();
  // if there is some data with command
  if( len )
  {
    memset( &t, 0x00, sizeof(t) );        // zero out the transaction
    t.length = len*8;                     // length is in bytes while transaction length is in bits
    t.tx_buffer = data;                   // The data is command itself
    t.user = (void*)SPI_USER_FLAG_DC_HIGH;// transaction id, keep it 1 for data mode
    ret = spi_device_polling_transmit(spi_tft_handle, &t);  // transmit
    TFT_CS_HIGH();
    assert(ret == ESP_OK);                // should have no issues
  }
  TFT_CS_HIGH();
}

/**
 * @brief Send Data to the TFT Controller
 *        Send Command to the LCD. Uses the "spi_device_polling_transmit", which
 *        waits until the transfer is complete.
 *        Since command transactions are usually small, they are handled in 
 *        polling mode for higher speed. The overhead of interrupt transaction 
 *        is more than just waiting for the transaction to complete.
 * @param data  data buffer pointer
 * @param len   length of the data
 */
void tft_send_data( const uint8_t *data, size_t len )
{
  esp_err_t ret;
  spi_transaction_t t;
  if( len == 0 )
    return;                                     // no need to send anything

  TFT_CS_LOW();
  TFT_DC_HIGH();
  memset( &t, 0x00, sizeof(t) );                // zero out the transaction
  t.length = len*8;                             // length is in bytes while transaction length is in bits
  t.tx_buffer = data;                           // Data
  t.user = (void*)SPI_USER_FLAG_FLUSH_READY;    // transaction id, keep it 1 for data mode
  // ret = spi_device_polling_transmit(spi_tft_handle, &t);  // transmit
  ret = spi_device_transmit(spi_tft_handle, &t);// transmit
  TFT_CS_HIGH();
  assert(ret == ESP_OK);                        // should have no issues
}

/**
 * @brief Read the data from the touch controller XPT2046
 * @param cmd   Command Value
 * @param data  Pointer to data
 * @param len   Length of the data which will be received
 */
void touch_read_data( uint8_t cmd, uint8_t *data, uint8_t len )
{
  esp_err_t ret;
  spi_transaction_t t;

  TOUCH_CS_LOW();
  memset( &t, 0x00, sizeof(t) );    // zero out the transaction
  t.length = (len * 8);
  t.rxlength = (len * 8);
  t.cmd = cmd;
  t.rx_buffer = data;
  // t.flags = SPI_TRANS_USE_RXDATA;    // if used this the received data will be in rx_data
  t.flags = 0;                          // recived data will be in rx_buffer and copied to *data buffer
  ret = spi_device_polling_transmit(spi_touch_handle, &t);  // transmit
  // ret = spi_device_transmit(spi_touch_handle, &t);  // transmit
  TOUCH_CS_HIGH();
  assert(ret == ESP_OK);
}

/**
 * @brief Return the TFT Width, considering the rotation factor
 * @param  None
 * @return Width
 */
uint16_t tft_get_width( void )
{
  return ili9341_get_width();
}

/**
 * @brief Return the TFT Height, considering the rotation factor
 * @param  None
 * @return Height
 */
uint16_t tft_get_height( void )
{
  return ili9341_get_height();
}

// Private Function Definitions

/**
 * @brief Initialize the TFT Drivere i.e. for ILI9341 and XPT2046 Controller
 * @param  None
 */
static void tft_driver_init( void )
{
  esp_err_t ret;
  // TODO: this needs to be evaluated that why it is not working when DMA is disabled
  spi_dma_chan_t dma_channel = SPI_DMA_CH1;   // don't enable DMA on Channel-0

  // SPI bus configuration for display
  // NOTE: the same bus configuration is used for touch also todo: check later
  spi_bus_config_t tft_bus_cfg =
  {
    .mosi_io_num = TFT_SPI_MOSI,
    .miso_io_num = TFT_SPI_MISO,
    .sclk_io_num = TFT_SPI_SCLK,
    // maximum transfer size in bytes, it is multiplied by 2 because we are using
    // RGB565 format which means two byte for single pixel and hence this is needed
    .max_transfer_sz = TFT_BUFFER_SIZE * 2,
    .quadhd_io_num = -1,
    .quadwp_io_num = -1,
  };

  // LCD Device configuration
  spi_device_interface_config_t tft_dev_config =
  {
    .clock_speed_hz = TFT_SPI_CLK_SPEED,
    .mode = 0,                      // SPI mode, representing pair of CPOL, CPHA
    .spics_io_num = TFT_SPI_CS,     // chip select for this device
    // .spics_io_num = -1,          // chip select for this device (for manual control)
    .input_delay_ns = 0,            // todo
    .queue_size = 50,               // Transaction queue size. This sets how
                                    // many transactions can be 'in the air'
                                    // (queued using spi_device_queue_trans
                                    // but not yet finished using
                                    // spi_device_get_trans_result) at the same time
    .pre_cb = tft_pre_tx_cb,        // callback to be called before transmission is started
    // .pre_cb = NULL,              // callback to be called before transmission is started
    .post_cb = tft_post_tx_cb,
    // .post_cb = NULL,                // callback to be called after transmission is completed
    .flags = SPI_DEVICE_NO_DUMMY,
  };

  // touch device config
  spi_device_interface_config_t touch_dev_config =
  {
    .clock_speed_hz = TOUCH_SPI_CLK_SPEED,
    .mode = 0,                      // SPI mode, representing pair of CPOL, CPHA
    .spics_io_num = TOUCH_SPI_CS,   // chip select for this device
    // .spics_io_num = -1,             // chip select for this device (manual control)
    .input_delay_ns = 0,            // todo
    .queue_size = 6,
    .pre_cb = NULL,                 // callback to be called before transmission is started
    .post_cb = NULL,                // callback to be called after transmission is completed
    .flags = SPI_DEVICE_NO_DUMMY,
    .command_bits = 8,              // for touch spi, we need one 8-bit command
  };

  // initialize the SPI bus
  ret = spi_bus_initialize(TFT_SPI_HOST, &tft_bus_cfg, dma_channel);
  assert(ret == ESP_OK);

  // attach the LCD to the SPI bus
  ret = spi_bus_add_device(TFT_SPI_HOST, &tft_dev_config, &spi_tft_handle);
  assert(ret == ESP_OK);

  // attach the touch to the SPI bus
  ret = spi_bus_add_device(TFT_SPI_HOST, &touch_dev_config, &spi_touch_handle);
  assert(ret == ESP_OK);

  //Initialize non-SPI GPIOs
  gpio_config_t io_conf = {};
  // Touch Chip Select and TFT Chip Select are now handled by SPI device configuration
  // Also the DC is not manually controlled, in-fact it is also handled in the
  // pre-transmission callback function
  // io_conf.pin_bit_mask = (1u<<TFT_PIN_DC) | (1u<<TFT_SPI_CS) | (1u<<TOUCH_SPI_CS);
  io_conf.pin_bit_mask = (1u<<TFT_PIN_DC);
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pull_up_en = true;
  gpio_config(&io_conf);

  TFT_CS_LOW();
  TFT_CS_HIGH();
  TOUCH_CS_LOW();
  TOUCH_CS_HIGH();
}

/**
 * @brief   Pre Transmission Callback
 *          This function is called (IRQ context) just before a transmission starts.
 *          It will set the D/C line to the value indicated in the user field.
 * @param t Transaction Handle
 */
static void tft_pre_tx_cb( spi_transaction_t *t )
{
  int flags = (int)t->user;
  switch(flags)
  {
    case SPI_USER_FLAG_DC_LOW:
      gpio_set_level(TFT_PIN_DC, 0);
      break;
    case SPI_USER_FLAG_DC_HIGH:
    case SPI_USER_FLAG_FLUSH_READY:
      gpio_set_level(TFT_PIN_DC, 1);
      break;
    default:
      break;
  };
}

/**
 * @brief   Post Transmission Callback
 *          This function is called (IRQ context) just after the transmission
 *          is completed.
 *          It will set call the flush ready function of the lvgl library
 * @param t Transaction Handle
 */
static void tft_post_tx_cb(spi_transaction_t *t )
{
  int flags = (int)t->user;
  switch(flags)
  {
    case SPI_USER_FLAG_FLUSH_READY:
      // todo:
      break;
    case SPI_USER_FLAG_DC_LOW:
    case SPI_USER_FLAG_DC_HIGH:
    default:
      break;
  };
}

