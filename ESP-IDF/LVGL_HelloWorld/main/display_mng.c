/*
 * display_mng.c
 *
 *  Created on: Dec 15, 2023
 *      Author: xpress_embedo
 */

#include "esp_heap_caps.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "lvgl.h"
#include "display_mng.h"

// Private Macros
#define DISP_SPI_HOST                 (SPI3_HOST)
#define DISP_SPI_CLK_SPEED            (40*1000000)

// Display Resolution
#define DISP_HOR_RES_MAX              (320)
#define DISP_VER_RES_MAX              (240)
#define DISP_BUFFER_SIZE              (DISP_HOR_RES_MAX * 40)

// Private Variables
// ---------------------------LVGL Related Stuff-------------------------------

// Private Function Prototypes
void display_driver_init( void );
void display_driver_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_map);

// Public Function Definitions
void display_init( void )
{
  // initialize lvgl library
  lv_init();

  // initialize the display drivers
  display_driver_init();


  // Buffer for 1/10 of the screen, we can increase the size of the buffer also
  // static lv_color_t buffer1[DISP_HOR_RES_MAX * 10];
  // Why using the below line, instead of the above, check the below link
  // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/mem_alloc.html
  lv_color_t* buffer1 = heap_caps_malloc(DISP_BUFFER_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
  assert(buffer1 != NULL);
  static lv_color_t *buffer2 = NULL;

  // LVGL Draw Buffer
  static lv_disp_draw_buf_t draw_buf;
  // initialize the working buffer
  lv_disp_draw_buf_init(&draw_buf, buffer1, buffer2, DISP_BUFFER_SIZE);

  // initialize the display
  static lv_disp_drv_t disp_drv;            // descriptor of the display driver
  lv_disp_drv_init(&disp_drv);              // basic initialization
  // change the following lines to your display resolution
  disp_drv.hor_res = DISP_HOR_RES_MAX;      // horizontal resolution
  disp_drv.ver_res = DISP_VER_RES_MAX;      // vertical resolution
  disp_drv.draw_buf = &draw_buf;            // assign the buffer to the display
  disp_drv.flush_cb = display_driver_flush; // driver function to flush display
  disp_drv.rotated = 0;                     // 0:=landscape & landscape inverted
  // disp_drv.rotated = 1;                     // 1:=portrait & portrait inverted
  lv_disp_drv_register( &disp_drv );        // finally register the driver

  // initialize the input device driver
  // TODO: XE when display started working
}

void display_mng( void )
{

}

// Private Function Definitions

void display_driver_init( void )
{
  esp_err_t ret;
  spi_dma_chan_t dma_channel = SPI_DMA_CH1;   // don't enable DMA on Channel-0

  spi_bus_config_t bus_cfg =
  {
      .mosi_io_num = DISP_SPI_MOSI,
      .miso_io_num = DISP_SPI_MISO,
      .sclk_io_num = DISP_SPI_SCLK,
      .max_transfer_sz = DISP_BUFFER_SIZE,  // maximum transfer size in bytes
      .quadhd_io_num = -1,
      .quadwp_io_num = -1,
  };

  // initialize the SPI bus
  spi_bus_initialize(DISP_SPI_HOST, &bus_cfg, dma_channel);
  assert(ret == ESP_OK);

  spi_device_interface_config_t dev_config =
  {
      .clock_speed_hz = DISP_SPI_CLK_SPEED,
      .mode = 0,                      // SPI mode, representing pair of CPOL, CPHA
      .spics_io_num = DISP_SPI_CS,    // chip select for this device
      .input_delay_ns = 0,            // todo
      .queue_size = 50,               // Transaction queue size. This sets how
                                      // many transactions can be 'in the air'
                                      // (queued using spi_device_queue_trans
                                      // but not yet finished using
                                      // spi_device_get_trans_result) at the same time
      .pre_cb = NULL,                 // callback to be called before transmission is started
      .post_cb = NULL,                // callback to be called after transmission is completed
      .flags = SPI_DEVICE_NO_DUMMY,
  };

  // define the SPI handle
  spi_device_handle_t spi_disp_handle;
  ret = spi_bus_add_device(DISP_SPI_HOST, &dev_config, &spi_disp_handle);
  assert(ret == ESP_OK);
}

void display_driver_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_map)
{

}
