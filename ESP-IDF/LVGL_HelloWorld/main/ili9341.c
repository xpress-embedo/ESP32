/*
 * ili9341.c
 *
 *  Created on: Dec 16, 2023
 *      Author: xpress_embedo
 */
#include "ili9341.h"

#include "tft.h"

// Defines
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
static void ili9341_send_cmd(uint8_t cmd, void * data, size_t length);
static void ili9341_send_data(void * data, size_t length);

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
    {ILI9341_POWERB, {0x00, 0xAA, 0XE0}, 3},
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

  tft_delay_ms(250);

  ili9341_sleep_out();

  tft_delay_ms(250);

  // Send all the commands
  uint16_t cmd = 0;
  while (ili_init_cmds[cmd].databytes!=0xff)
  {
    ili9341_send_cmd( ili_init_cmds[cmd].cmd, ili_init_cmds[cmd].data, (ili_init_cmds[cmd].databytes & 0x1F) );
    if (ili_init_cmds[cmd].databytes & 0x80)
    {
      tft_delay_ms(250);
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

uint16_t ili9341_get_width( void )
{
  return lcd_width;
}

uint16_t ili9341_get_height( void )
{
  return lcd_height;
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
    // Added this due to watchdog issue
    // vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

/**
 * @brief Draws a rectangle on lcd.
 * 
 * Draws a rectangle on lcd by using the specified parameters.
 * 
 * @param x_upper_left: x coordinate of the upper left rectangle corner. 
 * @param y_upper_left: y coordinate of the upper left rectangle corner. 
 * @param x_bottom_right: x coordinate of the lower right rectangle corner. 
 * @param y_bottom_right: y coordinate of the lower right rectangle corner. 
 * @param color: color parameter
 */
void ili9341_rectangle( int16_t x_upper_left, int16_t y_upper_left, int16_t x_bottom_right, int16_t y_bottom_right, uint16_t color)
{
  ili9341_draw_v_line( y_upper_left, y_bottom_right, x_upper_left, color);
  ili9341_draw_h_line( x_upper_left, x_bottom_right, y_bottom_right, color);
  ili9341_draw_v_line( y_upper_left, y_bottom_right, x_bottom_right, color);
  ili9341_draw_h_line( x_upper_left, x_bottom_right, y_upper_left, color);
}

void ili9341_fill_rectangle( int16_t x_start, int16_t y_start, int16_t x_end, int16_t y_end, uint16_t color )
{
  uint32_t total_pixels_to_write = 0u;
  uint8_t data[2] = { (color >> 8u), (color & 0xFF) };

  total_pixels_to_write = ((x_end+1u)-x_start) * ((y_end+1u)-y_start);
  if( total_pixels_to_write > ILI9341_PIXEL_COUNT )
  {
    total_pixels_to_write = ILI9341_PIXEL_COUNT;
  }

  ili9341_set_window( x_start, y_start, x_end, y_end );
  ili9341_send_cmd(ILI9341_GRAM, 0u, 0u );

  while( total_pixels_to_write )
  {
    ili9341_send_data( data, 2u );
    total_pixels_to_write--;
  }
}

/**
 * @brief Draws a Circle on ILI9341 LCD.
 * 
 * Draws a Circle on Glcd by using the specified parameters.
 * <a href="https://en.wikipedia.org/wiki/Midpoint_circle_algorithm">
 * Mid Point Circle Algorithm Weblink</a>
 * 
 * @param x_center: x coordinate of the circle center.
 * @param y_center: y coordinate of the circle center.
 * @param radius: radius of the circle.
 * @param color: color parameter. Valid values in format RGB565
 */
void ili9341_draw_circle( int16_t x_center, int16_t y_center, int16_t radius, uint16_t color)
{
  int16_t x = radius;
  int16_t y = 0;
  int16_t err = 0;
  int16_t temp1, temp2;
  
  while (x >= y)
  {
    temp1 = x_center + x;
    temp2 = y_center + y;
    if( (temp1 >=0) && (temp1 < lcd_width ) && \
        (temp2 >= 0) && (temp2 < lcd_height) )
    {
      ili9341_draw_pixel( temp1, temp2, color);
    }
    temp1 = x_center + y;
    temp2 = y_center + x;
    if( (temp1 >=0) && (temp1 < lcd_width) && \
        (temp2 >= 0) && (temp2 < lcd_height) )
    {
      ili9341_draw_pixel(temp1, temp2, color);
    }
    
    temp1 = x_center - y;
    temp2 = y_center + x;
    if( (temp1 >=0) && (temp1 < lcd_width) && \
        (temp2 >= 0) && (temp2 < lcd_height) )
    {
      ili9341_draw_pixel(temp1, temp2, color);
    }
    
    temp1 = x_center - x;
    temp2 = y_center + y;
    if( (temp1 >=0) && (temp1 < lcd_width) && \
        (temp2 >= 0) && (temp2 < lcd_height) )
    {
      ili9341_draw_pixel(temp1, temp2, color);
    }
    
    temp1 = x_center - x;
    temp2 = y_center - y;
    if( (temp1 >=0) && (temp1 < lcd_width) && \
        (temp2 >= 0) && (temp2 < lcd_height) )
    {
      ili9341_draw_pixel(temp1, temp2, color);
    }
    
    temp1 = x_center - y;
    temp2 = y_center - x;
    if( (temp1 >=0) && (temp1 < lcd_width) && \
        (temp2 >= 0) && (temp2 < lcd_height) )
    {
      ili9341_draw_pixel(temp1, temp2, color);
    }
    
    temp1 = x_center + y;
    temp2 = y_center - x;
    if( (temp1 >=0) && (temp1 < lcd_width) && \
        (temp2 >= 0) && (temp2 < lcd_height) )
    {
      ili9341_draw_pixel(temp1, temp2, color);
    }
    
    temp1 = x_center + x;
    temp2 = y_center - y;
    if( (temp1 >=0) && (temp1 < lcd_width) && \
        (temp2 >= 0) && (temp2 < lcd_height) )
    {
      ili9341_draw_pixel(temp1, temp2, color);
    }
    
    y += 1;
    err += 1 + 2*y;
    if (2*(err-x) + 1 > 0)
    {
      x -= 1;
      err += 1 - 2*x;
    }
  }
}

/**
 * @brief Draws a line on LCD.
 * 
 * Draws a line on LCD by using the specified parameters.
 * <a href="https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm">
 * Algorithm Used</a>
 * <a href="http://www.edaboard.com/thread68526.html#post302856"> Program Used
 * </a>
 * 
 * @param x_start: x coordinate of start point.
 * @param y_start: x coordinate of start point.
 * @param x_end: x coordinate of end point.
 * @param y_end: y coordinate of end point.
 * @param color: color parameter.
 */
void ili9341_draw_line( int16_t x_start, int16_t y_start, int16_t x_end, int16_t y_end, uint16_t color )
{
  int16_t x, y, addx, addy, dx, dy;
  int32_t P;
  int16_t i;
  dx = abs((int16_t)(x_end - x_start));
  dy = abs((int16_t)(y_end - y_start));
  x = x_start;
  y = y_start;
  
  if(x_start > x_end)
    addx = -1;
  else
    addx = 1;
  
  if(y_start > y_end)
    addy = -1;
  else
    addy = 1;
  
  if(dx >= dy)
  {
    P = 2*dy - dx;
    
    for(i=0; i<=dx; ++i)
    {
      ili9341_draw_pixel( x, y, color );
      if(P < 0)
      {
        P += 2*dy;
        x += addx;
      }
      else
      {
        P += 2*dy - 2*dx;
        x += addx;
        y += addy;
      }
    }
  }
  else
  {
    P = 2*dx - dy;
    for(i=0; i<=dy; ++i)
    {
      ili9341_draw_pixel( x, y, color );
      
      if(P < 0)
      {
        P += 2*dx;
        y += addy;
      }
      else
      {
        P += 2*dx - 2*dy;
        x += addx;
        y += addy;
      }
    }
  }
}

void ili9341_draw_h_line( int16_t x_start, int16_t y_start, int16_t width, uint16_t color )
{
  ili9341_draw_line( x_start, y_start, (x_start+width-1u), y_start, color);
}

void ili9341_draw_v_line( int16_t x_start, int16_t y_start, int16_t height, uint16_t color )
{
  ili9341_draw_line( x_start, y_start, x_start, (y_start+height-1u), color);
}

void ili9341_draw_triangle( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  ili9341_draw_line(x0, y0, x1, y1, color);
  ili9341_draw_line(x1, y1, x2, y2, color);
  ili9341_draw_line(x2, y2, x0, y0, color);
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


static void ili9341_send_cmd(uint8_t cmd, void * data, size_t length)
{
  tft_send_cmd(cmd, data, length);
}

static void ili9341_send_data(void * data, size_t length)
{
  tft_send_data(data, length);
}


