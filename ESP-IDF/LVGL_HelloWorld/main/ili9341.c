/*
 * ili9341.c
 *
 *  Created on: Dec 16, 2023
 *      Author: xpress_embedo
 */
#include "ili9341.h"
#include "display_mng.h"
#include "driver/gpio.h"
#include "rom/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Defines
#define TAG "ILI9341"

#define ILI9341_MADCTL_MY           (0x80u)   // Bottom to top
#define ILI9341_MADCTL_MX           (0x40u)   // Right to left
#define ILI9341_MADCTL_MV           (0x20u)   // Reverse Mode
#define ILI9341_MADCTL_ML           (0x10u)   // LCD refresh Bottom to top
#define ILI9341_MADCTL_RGB          (0x00u)   // Led-Green-Blue pixel order
#define ILI9341_MADCTL_BGR          (0x08u)   // Blue-Green-Red pixel order
#define ILI9341_MADCTL_MH           (0x04u)   // LCD refresh right to left

// structures
// The LCD needs a bunch of command/argument values to be initialized. They are stored in this struct
typedef struct
{
  uint8_t cmd;
  uint8_t data[16];
  uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;

// Private Function Prototypes
static void ili9341_reset(void);
static void ili9341_sleep_out(void);
static void ili9341_send_cmd(uint8_t cmd, void * data, uint16_t length);
static void ili9341_send_data(void * data, uint16_t length);

// Private Variables
static ili9341_orientation_e lcd_orientation = LCD_PORTRAIT;
static uint16_t lcd_width = ILI9341_LCD_WIDTH;
static uint16_t lcd_height = ILI9341_LCD_HEIGHT;

// Public Function Definition
void ili9341_init( void )
{
  lcd_init_cmd_t ili_init_cmds[]=
  {
    /* This is an un-documented command
    https://forums.adafruit.com/viewtopic.php?f=47&t=63229&p=320378&hilit=0xef+ili9341#p320378
    */
    // {0xEF, {0x03, 0x80, 0x02}, 3},
    //  {cmd, { data }, data_size}
    {ILI9341_MAC, {0x08}, 1},
    {ILI9341_PIXEL_FORMAT, {0x55}, 1},
    /* Power contorl B, power control = 0, DC_ENA = 1 */
    {0xCF, {0x00, 0xAA, 0XE0}, 3},
    /* Power on sequence control,
     * cp1 keeps 1 frame, 1st frame enable
     * vcl = 0, ddvdh=3, vgh=1, vgl=2
     * DDVDH_ENH=1
     */
    {0xED, {0x67, 0x03, 0X12, 0X81}, 4},
    /* Driver timing control A,
     * non-overlap=default +1
     * EQ=default - 1, CR=default
     * pre-charge=default - 1
     */
    {0xE8, {0x8A, 0x01, 0x78}, 3},
    /* Power control A, Vcore=1.6V, DDVDH=5.6V */
    {0xCB, {0x39, 0x2C, 0x00, 0x34, 0x02}, 5},
    /* Pump ratio control, DDVDH=2xVCl */
    {0xF7, {0x20}, 1},

    {0xF7, {0x20}, 1},
    /* Driver timing control, all=0 unit */
    {0xEA, {0x00, 0x00}, 2},
    /* Power control 1, GVDD=4.75V */
    {0xC0, {0x23}, 1},
    /* Power control 2, DDVDH=VCl*2, VGH=VCl*7, VGL=-VCl*3 */
    {0xC1, {0x11}, 1},
    /* VCOM control 1, VCOMH=4.025V, VCOML=-0.950V */
    {0xC5, {0x43, 0x4C}, 2},
    /* VCOM control 2, VCOMH=VMH-2, VCOML=VML-2 */
    {0xC7, {0xA0}, 1},
    /* Frame rate control, f=fosc, 70Hz fps */
    {0xB1, {0x00, 0x1B}, 2},
    /* Enable 3G, disabled */
    {0xF2, {0x00}, 1},
    /* Gamma set, curve 1 */
    {0x26, {0x01}, 1},
    /* Positive gamma correction */
    {0xE0, {0x1F, 0x36, 0x36, 0x3A, 0x0C, 0x05, 0x4F, 0X87, 0x3C, 0x08, 0x11, 0x35, 0x19, 0x13, 0x00}, 15},
    /* Negative gamma correction */
    {0xE1, {0x00, 0x09, 0x09, 0x05, 0x13, 0x0A, 0x30, 0x78, 0x43, 0x07, 0x0E, 0x0A, 0x26, 0x2C, 0x1F}, 15},
    /* Entry mode set, Low vol detect disabled, normal display */
    {0xB7, {0x07}, 1},
    /* Display function control */
    {0xB6, {0x08, 0x82, 0x27}, 3},    /* Display on */
    {ILI9341_MAC, {0x48}, 1},
    {0x29, {0}, 0x80},
    {0x00, {0}, 0xff},
  };

  ili9341_reset();

  vTaskDelay(250 / portTICK_PERIOD_MS);

  ili9341_sleep_out();

  vTaskDelay(250 / portTICK_PERIOD_MS);

  ESP_LOGI(TAG, "LCD ILI9341 Initialization.");

  // Send all the commands
  uint16_t cmd = 0;
  while (ili_init_cmds[cmd].databytes!=0xff)
  {
    ili9341_send_cmd( ili_init_cmds[cmd].cmd, ili_init_cmds[cmd].data, (ili_init_cmds[cmd].databytes & 0x1F) );
    if (ili_init_cmds[cmd].databytes & 0x80)
    {
      vTaskDelay(200 / portTICK_PERIOD_MS);
    }
    cmd++;
    }
  }

void ili9341_set_orientation( ili9341_orientation_e orientation )
{
  uint8_t data = 0x00;

  switch (orientation)
  {
  case LCD_ORIENTATION_0:
    data = (ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
    lcd_width = ILI9341_LCD_WIDTH;
    lcd_height = ILI9341_LCD_HEIGHT;
    break;
  case LCD_ORIENTATION_90:
    data = (ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
    lcd_width = ILI9341_LCD_HEIGHT;
    lcd_height = ILI9341_LCD_WIDTH;
    break;
  case LCD_ORIENTATION_180:
    data = (ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR);
    lcd_width = ILI9341_LCD_WIDTH;
    lcd_height = ILI9341_LCD_HEIGHT;
    break;
  case LCD_ORIENTATION_270:
    data = (ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
    lcd_width = ILI9341_LCD_HEIGHT;
    lcd_height = ILI9341_LCD_WIDTH;
    break;
  default:
    orientation = LCD_ORIENTATION_0;
    data = (ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
    break;
  }
  lcd_orientation = orientation;
  ili9341_send_cmd(ILI9341_MAC, &data, 1);
}

ili9341_orientation_e ili9341_get_orientation( void )
{
  return lcd_orientation;
}

// Set the display area
void ili9341_set_window( uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end )
{
  uint8_t params[4] = { 0 };
  // column address set
  params[0] = x_start >> 8u;
  params[1] = 0xFF & x_start;
  params[2] = x_end >> 8u;
  params[3] = 0xFF & x_end;
  ili9341_send_cmd(ILI9341_CASET, params, 4u );

  // Row Address Set (2B) also called as page address set
  params[0] = y_start >> 8u;
  params[1] = 0xFF & y_start;
  params[2] = y_end >> 8u;
  params[3] = 0xFF & y_end;
  ili9341_send_cmd( ILI9341_RASET, params, 4u );
}

void ili9341_draw_pixel( uint16_t x, uint16_t y, uint16_t color )
{
  uint8_t data[2] = { (color>>8u), (color & 0xFF) };
  ili9341_set_window( x, y, x, y);
  ili9341_send_cmd(ILI9341_GRAM, data, 2u );
}

void ili9341_fill( uint16_t color )
{
  uint32_t total_pixel_counts = ILI9341_PIXEL_COUNT;
  uint8_t data[2] = { (color >> 8u), (color & 0xFF) };

  ili9341_set_window( 0u, 0u, (lcd_width-1u), (lcd_height-1u) );
  ili9341_send_cmd(ILI9341_GRAM, 0u, 0u );
  while( total_pixel_counts )
  {
    ili9341_send_data( data, 2u );
    total_pixel_counts--;
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

// Private Function Definitions
static void ili9341_reset(void)
{
  // ili9341 software reset command
  ili9341_send_cmd(ILI9341_SWRESET, 0, 0);
}

static void ili9341_sleep_out(void)
{
  ili9341_send_cmd(ILI9341_SLEEP_OUT, 0, 0);
}


static void ili9341_send_cmd(uint8_t cmd, void * data, uint16_t length)
{
  display_send_cmd(cmd, data, length);
}

static void ili9341_send_data(void * data, uint16_t length)
{
  display_send_data(data, length);
}


