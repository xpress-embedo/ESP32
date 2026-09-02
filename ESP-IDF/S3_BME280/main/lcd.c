/*
 * lcd.c
 *
 *  Created on: Mar 2, 2024
 *      Author: xpress_embedo
 */

#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_io_i2c.h"
#include "esp_lcd_touch.h"
#include "esp_lcd_touch_gt911.h"
#include "esp_heap_caps.h"
#include <assert.h>

#include "lvgl.h"

#include "lcd.h"

// Private Macros
// 2 ms tick gives smooth LVGL timing while keeping timer overhead low.
#define LV_TICK_PERIOD_MS                           (2)
// I2C address of GT911 touch controller on this Sunton board.
#define TOUCH_IO_I2C_GT911_ADDRESS                  (0x5D)

// Private Function Prototypes
// These helpers keep lcd_init readable by splitting work into small steps.
static uint16_t lcd_map_range_u16( uint16_t value, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max );
static esp_err_t i2c_init( void );
static void gt911_touch_init( esp_lcd_touch_handle_t *tp );
static void gt911_process( esp_lcd_touch_handle_t tp, uint16_t *x, uint16_t *y, uint16_t *strength, uint8_t *point_num, uint8_t max_point_num );
static void gt911_touchpad_read( lv_indev_t *indev, lv_indev_data_t *data );
static void lcd_flush_cb( lv_display_t *display, const lv_area_t *area, uint8_t *px_map );
static void lvgl_tick( void *arg );

// Private Variables
// Handles are saved globally so callbacks can use them later.
static const char *TAG = "LCD";
const i2c_port_t I2C_PORT = I2C_NUM_0;
static i2c_master_bus_handle_t s_i2c_bus = NULL;
static esp_lcd_panel_handle_t s_panel_handle = NULL;
static esp_lcd_touch_handle_t s_tp = NULL;
static lv_display_t *s_display = NULL;
static lv_indev_t *s_touch_indev = NULL;

// Public Function Definition
/**
 * @brief Initialize RGB panel, LVGL display buffers, LVGL tick source, and GT911 touch input.
 *
 * Beginner idea:
 * Think of this as "power up all display-related parts in the correct order".
 * If the order is wrong, screen may stay blank or touch may not work.
 *
 * Init order is important:
 * 1) configure panel and backlight GPIO
 * 2) create/init RGB panel
 * 3) init LVGL and display buffers
 * 4) start LVGL tick timer
 * 5) init touch bus/controller and register LVGL input device
 * 6) enable backlight after all subsystems are ready
 * @param None
 */
void lcd_init( void )
{
  // Backlight pin is configured first so we can keep panel dark while booting.
  gpio_config_t backlight_config = {
    .pin_bit_mask = 1u << LCD_PIN_BK_LIGHT,
    .mode         = GPIO_MODE_OUTPUT,
    .pull_up_en   = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type    = GPIO_INTR_DISABLE,
  };

  // This structure tells ESP-IDF how your 800x480 RGB panel is wired and timed.
  esp_lcd_rgb_panel_config_t panel_config = {
    .clk_src                = LCD_CLK_SRC_DEFAULT,
    .timings = {
      .pclk_hz              = LCD_PIXEL_CLOCK_HZ,
      .h_res                = LCD_H_RES,
      .v_res                = LCD_V_RES,
      .hsync_pulse_width    = 4,
      .hsync_back_porch     = 8,
      .hsync_front_porch    = 8,
      .vsync_pulse_width    = 4,
      .vsync_back_porch     = 8,
      .vsync_front_porch    = 8,
      .flags = {
        .hsync_idle_low   = false,
        .vsync_idle_low   = false,
        .de_idle_high     = false,
        .pclk_active_neg  = true,
        .pclk_idle_high   = false
      },
    },
    .data_width             = 16,
    .dma_burst_size         = 64,
    .num_fbs                = 2,
    .bounce_buffer_size_px  = 0,

    .hsync_gpio_num         = LCD_PIN_HSYNC,
    .vsync_gpio_num         = LCD_PIN_VSYNC,
    .de_gpio_num            = LCD_PIN_DE,
    .pclk_gpio_num          = LCD_PIN_PCLK,
    .disp_gpio_num          = LCD_PIN_DISP_EN,
    .data_gpio_nums = {
      LCD_PIN_DATA0,
      LCD_PIN_DATA1,
      LCD_PIN_DATA2,
      LCD_PIN_DATA3,
      LCD_PIN_DATA4,
      LCD_PIN_DATA5,
      LCD_PIN_DATA6,
      LCD_PIN_DATA7,
      LCD_PIN_DATA8,
      LCD_PIN_DATA9,
      LCD_PIN_DATA10,
      LCD_PIN_DATA11,
      LCD_PIN_DATA12,
      LCD_PIN_DATA13,
      LCD_PIN_DATA14,
      LCD_PIN_DATA15
    },
    .flags = {
      .disp_active_low      = 0,
      .refresh_on_demand    = 0,
      .fb_in_psram          = true,
      .double_fb            = true,
      .no_fb                = 0,
      .bb_invalidate_cache  = 0,
    }
  };

  // Keep backlight OFF during setup to avoid flash/noise on screen at startup.
  ESP_LOGI(TAG, "Turn off LCD Back-Light");
  ESP_ERROR_CHECK( gpio_config(&backlight_config) );

  // Create driver object for RGB peripheral.
  ESP_LOGI(TAG, "Install RGB LCD panel driver");
  ESP_ERROR_CHECK( esp_lcd_new_rgb_panel(&panel_config, &s_panel_handle) );

  // Reset + init programs LCD peripheral with timings and starts panel path.
  ESP_LOGI(TAG, "Initialize RGB LCD panel");
  ESP_ERROR_CHECK( esp_lcd_panel_reset(s_panel_handle) );
  ESP_ERROR_CHECK( esp_lcd_panel_init(s_panel_handle) );

  // Initialize LVGL core before creating displays/objects.
  ESP_LOGI(TAG, "Initialize LVGL library");
  lv_init();

  /*
   * Create one LVGL display with physical resolution.
   * LVGL now knows width/height of your real screen.
   */
  s_display = lv_display_create( LCD_H_RES, LCD_V_RES );

  /*
   * LVGL needs temporary draw buffers.
   * We keep them in PSRAM (not DRAM) because ESP32-S3 DRAM is limited.
   * 40 lines means LVGL draws screen in slices to save memory.
   */
  const uint32_t buf_lines = 40;
  const size_t draw_buf_pixels = LCD_H_RES * buf_lines;
  const size_t draw_buf_bytes = draw_buf_pixels * sizeof(lv_color16_t);
  lv_color16_t *disp_buf1 = esp_lcd_rgb_alloc_draw_buffer(s_panel_handle, draw_buf_bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  lv_color16_t *disp_buf2 = esp_lcd_rgb_alloc_draw_buffer(s_panel_handle, draw_buf_bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  // assert stops here in debug if memory allocation fails.
  assert(disp_buf1 && disp_buf2);

  lv_display_set_buffers(
    s_display,
    disp_buf1,
    disp_buf2,
    draw_buf_bytes,
    LV_DISPLAY_RENDER_MODE_PARTIAL
  );

  /*
   * Flush callback = bridge between LVGL and LCD hardware.
   * LVGL renders pixels; this callback sends them to panel.
   */
  lv_display_set_flush_cb( s_display, lcd_flush_cb );

  // LVGL uses a software time base. We provide it by periodic esp_timer tick.
  const esp_timer_create_args_t lvgl_tick_timer_args =
  {
    .callback = &lvgl_tick,
    .name = "lvgl_tick"
  };
  esp_timer_handle_t lvgl_tick_timer;
  ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
  // esp_timer period is microseconds, hence x1000 conversion from ms.
  ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LV_TICK_PERIOD_MS * 1000));

  // Touch path: create I2C master bus first, then create GT911 device driver.
  ESP_ERROR_CHECK( i2c_init() );
  gt911_touch_init(&s_tp);

  // Register touch as LVGL pointer input so buttons/sliders can receive taps.
  s_touch_indev = lv_indev_create();
  lv_indev_set_type( s_touch_indev, LV_INDEV_TYPE_POINTER );
  lv_indev_set_read_cb( s_touch_indev, gt911_touchpad_read );
  lv_indev_set_user_data( s_touch_indev, s_tp );

  // Final step: turn backlight on after all software components are ready.
  lcd_set_backlight(true);
}


/**
 * @brief Turn On/Off the Backlight
 *        true  -> screen light ON
 *        false -> screen light OFF
 * @param state true means turn on and false means off
 */
void lcd_set_backlight( bool state )
{
  if( state )
  {
    gpio_set_level(LCD_PIN_BK_LIGHT, LCD_BK_LIGHT_ON_LEVEL);
  }
  else
  {
    gpio_set_level(LCD_PIN_BK_LIGHT, LCD_BK_LIGHT_OFF_LEVEL);
  }
}

// Private Function Definition
/**
 * @brief Map one 16-bit value range into another 16-bit value range.
 *
 * Example:
 * If touch raw X is 0..477, but screen X is 0..800,
 * this function converts one range to the other.
 *
 * @param value Input value to map.
 * @param in_min Minimum of input range.
 * @param in_max Maximum of input range.
 * @param out_min Minimum of output range.
 * @param out_max Maximum of output range.
 * @return uint16_t Mapped value in output range.
 */
static uint16_t lcd_map_range_u16( uint16_t value, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max )
{
  return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/**
 * @brief Initialize new ESP-IDF I2C master bus API used by the touch controller.
 *
 * This creates one I2C bus object that later gets passed to the GT911 IO layer.
 * @param none
 */
static esp_err_t i2c_init( void )
{
  i2c_master_bus_config_t config = {
    .i2c_port = I2C_PORT,
    .sda_io_num = TOUCH_PIN_SDA,
    .scl_io_num = TOUCH_PIN_SCL,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
    .flags = {
      .enable_internal_pullup = true,
    },
  };

  ESP_LOGI(TAG, "Initializing I2C master bus for touch");

  return i2c_new_master_bus( &config, &s_i2c_bus );
}

/**
 * @brief Initialize GT911 driver over LCD panel I2C IO wrapper.
 *
 * Two layers are created here:
 * 1) panel IO over I2C (low-level read/write channel)
 * 2) GT911 touch driver using that channel
 * @param esp_lcd_touch_handle_t pointer to touch handle
 */
static void gt911_touch_init( esp_lcd_touch_handle_t *tp )
{
  esp_lcd_panel_io_handle_t tp_io_handle = NULL;

  const esp_lcd_panel_io_i2c_config_t tp_io_config = {
    .dev_addr = TOUCH_IO_I2C_GT911_ADDRESS,
    .scl_speed_hz = TOUCH_FREQ_HZ,
    .on_color_trans_done = NULL,
    .user_ctx = NULL,
    .control_phase_bytes = 1,
    .dc_bit_offset = 0,
    .lcd_cmd_bits = 16,
    .lcd_param_bits = 0,
    .flags = {
      .dc_low_on_data = 0,
      .disable_control_phase = 1,
    },
  };

  const esp_lcd_touch_config_t tp_cfg = {
    .x_max = LCD_H_RES,
    .y_max = LCD_V_RES,
    .rst_gpio_num = TOUCH_PIN_RESET,
    .int_gpio_num = TOUCH_PIN_INT,
    .levels = {
      .reset = 0,
      .interrupt = 0,
    },
    .flags = {
      .swap_xy = 0,
      .mirror_x = 0,
      .mirror_y = 0,
    },
    // Optional callback to convert raw coordinates into display coordinates.
    .process_coordinates = gt911_process,
    .interrupt_callback = NULL
  };

  ESP_ERROR_CHECK( esp_lcd_new_panel_io_i2c(s_i2c_bus, &tp_io_config, &tp_io_handle) );
  ESP_ERROR_CHECK( esp_lcd_touch_new_i2c_gt911(tp_io_handle, &tp_cfg, tp) );
}

/**
 * @brief Coordinate post-processing callback used by esp_lcd_touch.
 *        Converts raw controller range into LCD pixel range.
 * @param esp_lcd_touch_handle_t touch handle
 * @param x pointer to x coordinate
 * @param y pointer to y coordinate
 * @prama strength
 * @param point_num
 * @param max_point_num
 */
static void gt911_process( esp_lcd_touch_handle_t tp, uint16_t *x, uint16_t *y, uint16_t *strength, uint8_t *point_num, uint8_t max_point_num )
{
  (void) tp;
  (void) strength;
  (void) point_num;
  (void) max_point_num;

  // We map raw ranges from controller space into actual LCD pixel space.
  *x = lcd_map_range_u16(*x, TOUCH_H_RES_MIN, TOUCH_H_RES_MAX, 0, LCD_H_RES);
  *y = lcd_map_range_u16(*y, TOUCH_V_RES_MIN, TOUCH_V_RES_MAX, 0, LCD_V_RES);
}

/**
 * @brief LVGL input callback. Reads latest touch sample and fills LVGL pointer state.
 * @param indev pointer to LVGL input device object
 * @param data pointer to LVGL touch sample output
 */
static void gt911_touchpad_read( lv_indev_t *indev, lv_indev_data_t *data )
{
  // Retrieve touch driver handle we stored in lv_indev user_data.
  esp_lcd_touch_handle_t tp = (esp_lcd_touch_handle_t)lv_indev_get_user_data(indev);
  assert(tp);

  esp_lcd_touch_point_data_t touch_points[1] = {0};
  uint8_t touchpad_cnt = 0;

  // Ask controller driver to fetch latest sample from hardware.
  esp_lcd_touch_read_data(tp);

  // Convert touch sample into LVGL input format.
  if( esp_lcd_touch_get_data(tp, touch_points, &touchpad_cnt, 1) == ESP_OK && touchpad_cnt > 0 )
  {
    data->point.x = touch_points[0].x;
    data->point.y = touch_points[0].y;
    data->state = LV_INDEV_STATE_PRESSED;   // Finger is touching the panel
  }
  else
  {
    data->state = LV_INDEV_STATE_RELEASED;  // No touch currently
  }
}

/**
 * @brief LVGL flush callback. Copies rendered area to RGB panel frame buffer.
 * @param display LVGL display object
 * @param area dirty area in display coordinates
 * @param px_map rendered pixel data for the dirty area
 */
static void lcd_flush_cb( lv_display_t *display, const lv_area_t *area, uint8_t *px_map )
{
  (void) display;

  // LVGL provides an updated rectangle; we pass that rectangle to panel driver.
  int offsetx1 = area->x1;
  int offsetx2 = area->x2;
  int offsety1 = area->y1;
  int offsety2 = area->y2;

  esp_lcd_panel_draw_bitmap(
    s_panel_handle,
    offsetx1,
    offsety1,
    offsetx2 + 1,
    offsety2 + 1,
    px_map
  );
  lv_display_flush_ready(display);
}

/**
 * @brief esp_timer callback that increments LVGL tick counter.
 * @param arg
 */
static void lvgl_tick(void *arg)
{
  (void) arg;
  // Increment LVGL internal time by fixed period.
  lv_tick_inc(LV_TICK_PERIOD_MS);
}

/**
 * @brief Run one LVGL handler cycle.
 *
 * This function should be called periodically from a task context.
 * It drives LVGL internals (redraw, animation, input processing).
 */
void lcd_lvgl_timer_handler( void )
{
  lv_lock();
  // LVGL engine: processes animations, input, and redraw scheduling.
  // Call this periodically from a FreeRTOS task.
  lv_timer_handler();
  lv_unlock();
}
