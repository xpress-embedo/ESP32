/*
 * display_mng.c
 *
 *  Created on: Jan 26, 2024
 *      Author: xpress_embedo
 */

#include "esp_heap_caps.h"
#include "esp_timer.h"

#include "lvgl.h"
#include "display_mng.h"

// Defines
#define LV_TICK_PERIOD_MS           (2)
#define DISP_BUFFER_SIZE            (TFT_BUFFER_SIZE)

// Private Function Declarations
static void display_flush_slow_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);
static void display_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);
static void display_flush_swap_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);
static void lvgl_tick(void *arg);

// Public Function Definitions
void display_init( void )
{
  // initialize the lvgl library
  lv_init();

  // initialize the tft and touch library
  tft_init();
  xpt2046_init();

#if 0
  // LVGL version 9
  uint16_t* buf1 = heap_caps_malloc(DISP_BUFFER_SIZE * sizeof(uint16_t) , MALLOC_CAP_DMA);
  assert(buf1 != NULL);

  static lv_color_t* buf2 = NULL;

  lv_display_t * display = lv_display_create( tft_get_width(), tft_get_height() );
  tft_delay_ms(10);
  lv_display_set_buffers(display, buf1, buf2, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(display, display_flush_cb);
#endif

#if 1
  // for LVGL 8.3.11
  static lv_disp_draw_buf_t draw_buf; // contains internal graphics buffer called draw buffer
  static lv_disp_drv_t disp_drv;      // contains callback functions


  // Buffer for 1/10 of the screen, we can increase the size of the buffer also
  // static lv_color_t buf1[DISP_HOR_RES_MAX * 10];
  // Why using the below line, instead of the above, check the below link
  // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/mem_alloc.html
  lv_color_t *buf1 = heap_caps_malloc(DISP_BUFFER_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
  assert(buf1);
  lv_color_t *buf2 = heap_caps_malloc(DISP_BUFFER_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
  assert(buf2);
  // LVGL Draw Buffer

  // initialize the working buffer
  lv_disp_draw_buf_init(&draw_buf, buf1, buf2, DISP_BUFFER_SIZE);

  // Register Display Driver to LVGL
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = tft_get_width();
  disp_drv.ver_res = tft_get_height();
  // disp_drv.flush_cb = display_flush_cb;
  // disp_drv.flush_cb = display_flush_slow_cb;
  disp_drv.flush_cb = display_flush_swap_cb;
  disp_drv.drv_update_cb = NULL;        // todo
  disp_drv.draw_buf = &draw_buf;
  // user data todo
  // lv_disp_t *disp = lv_disp_drv_register(&disp_drv);
  lv_disp_drv_register(&disp_drv);
#endif

  // Tick Interface for LVGL using esp_timer to generate 2ms periodic event
  const esp_timer_create_args_t lvgl_tick_timer_args =
  {
    .callback = &lvgl_tick,
    .name = "lvgl_tick"
  };
  esp_timer_handle_t lvgl_tick_timer;
  ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
  ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LV_TICK_PERIOD_MS * 1000));  // here time is in micro seconds

  // configuring input devices
  // todo
}


// Private Function Definitions
// This function is so slow that it will create a watchdog reset
static void display_flush_slow_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
  uint16_t x, y;
  lv_color_t temp;
  for(y = area->y1; y <= area->y2; y++)
  {
    for(x = area->x1; x <= area->x2; x++)
    {
      temp = *color_map;
      ili9341_draw_pixel(x, y, temp.full);
      color_map++;
    }
  }
  lv_disp_flush_ready(drv);
}

static void display_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
  size_t idx = 0;
  ili9341_set_window(area->x1, area->y1, area->x2, area->y2);
  // transfer frame buffer
  // size_t len = ((area->x2+1 - area->x1) * (area->y2+1 - area->y1) * 2);
  // tft_send_cmd(ILI9341_GRAM, (uint8_t*)color_map, len);

  uint8_t data[2];
  uint16_t temp;
  size_t len = ((area->x2+1 - area->x1) * (area->y2+1 - area->y1));
  tft_send_cmd(ILI9341_GRAM, 0, 0);
  for( idx = 0; idx < len; idx++ )
  {
    temp = color_map->full;
    data[0] = (temp)>>8;
    data[1] = (temp) & 0xFF;
    tft_send_data(data, 2);
    color_map++;
  }

  lv_disp_flush_ready(drv);
}


static void display_flush_swap_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
  size_t idx = 0;
  size_t width = (area->x2 - area->x1 + 1);
  size_t height = (area->y2 - area->y1 + 1);
  size_t len = width * height;
  uint8_t data[2];
  uint16_t temp;

  ili9341_set_window(area->x1, area->y1, area->x2, area->y2);
  // transfer frame buffer
  // size_t len = ((area->x2+1 - area->x1) * (area->y2+1 - area->y1) * 2);
  // tft_send_cmd(ILI9341_GRAM, (uint8_t*)color_map, len);

  tft_send_cmd(ILI9341_GRAM, 0, 0);
  for( idx = 0; idx < len; idx++ )
  {
    temp = color_map->full;
    data[0] = (temp)>>8;
    data[1] = (temp) & 0xFF;
    tft_send_data(data, 2);
    color_map++;
  }

  lv_disp_flush_ready(drv);
}

static void lvgl_tick(void *arg)
{
  (void) arg;

  lv_tick_inc(LV_TICK_PERIOD_MS);
}
