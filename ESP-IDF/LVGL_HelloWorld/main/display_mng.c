/*
 * display_mng.c
 *
 *  Created on: Dec 15, 2023
 *      Author: xpress_embedo
 */

#include "esp_heap_caps.h"
#include "lvgl.h"
#include "display_mng.h"

// Private Macros
// Display Related SPI Pins
#define DISP_SPI_MOSI
#define DISP_SPI_MISO
#define DISP_SPI_SCLK
#define DISP_SPI_CS
// Touch Related SPI Pins (NOTE: Display and Touch SPI is considered as same,
// only chip select is different for obvious reasons
#define TOUCH_SPI_MOSI                (DISP_SPI_MOSI)
#define TOUCH_SPI_MISO                (DISP_SPI_MISO)
#define TOUCH_SPI_SCLK                (DISP_SPI_SCLK)
#define TOUCH_SPI_CS

// Display Resolution
#define DISP_HOR_RES_MAX              (320u)
#define DISP_VER_RES_MAX              (240u)
#define DISP_BUFFER_SIZE              (DISP_HOR_RES_MAX * 40u)

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

}

void display_driver_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_map)
{

}
