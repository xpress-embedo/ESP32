/*
 * xpt2046.c
 *
 *  Created on: Jan 20, 2024
 *      Author: xpress_embedo
 */

#include "tft.h"
#include "xpt2046.h"

// Defines

#define CMD_X_READ                  0b10010000  // NOTE: XPT2046 data sheet says this is actually Y 0x90
#define CMD_Y_READ                  0b11010000  // NOTE: XPT2046 data sheet says this is actually X 0xD0
#define CMD_Z1_READ                 0b10110000  // 0xB0
#define CMD_Z2_READ                 0b11000000  // 0xC0
#define XPT2046_TOUCH_THRESHOLD     400 // Threshold for touch detection
#define XPT2046_AVG                 4
#define XPT2046_X_MIN               200
#define XPT2046_Y_MIN               200
#define XPT2046_X_MAX               1950
#define XPT2046_Y_MAX               1860
#define XPT2046_X_INV               0
#define XPT2046_Y_INV               0
#define XPT2046_XY_SWAP             0


typedef enum {
  TOUCH_NOT_DETECTED = 0,
  TOUCH_DETECTED,
} xpt2046_touch_detect_t;

// Private Function Prototypes
static void xpt2046_avg(int16_t * x, int16_t * y);
static int16_t xpt2046_cmd(uint8_t cmd);
static xpt2046_touch_detect_t xpt2048_is_touch_detected(void);
static void xpt2046_corr(int16_t * x, int16_t * y);

// Private Variables
int16_t avg_buf_x[XPT2046_AVG];
int16_t avg_buf_y[XPT2046_AVG];
uint8_t avg_last;

// Public Function Definition
void xpt2046_init(void)
{

}

void xpt2046_read(void)
{
  static int16_t last_x = 0;
  static int16_t last_y = 0;
  bool valid = false;

  int16_t x = last_x;
  int16_t y = last_y;

  if( xpt2048_is_touch_detected() == TOUCH_DETECTED )
  {
    valid = true;
    xpt2046_cmd(CMD_X_READ);
    x = xpt2046_cmd(CMD_X_READ);
    tft_delay_ms(100);
    y = xpt2046_cmd(CMD_Y_READ);
    // printf("\nX = %d, Y = %d\n", x, y);

    // Normalize Data back to 12-bits
    x = x >> 4;
    y = y >> 4;
    printf("Normalize X = %d, Y = %d\n", x, y);

    xpt2046_corr(&x, &y);
    xpt2046_avg(&x, &y);

    last_x = x;
    last_y = y;
    // printf("Calculated X = %d, Y = %d\n\n", x, y);
  }
  else
  {
    avg_last = 0;
  }
}

// Private Function Definitions
static void xpt2046_avg(int16_t * x, int16_t * y)
{
  // Shift out the oldest data
  uint8_t i;
  for(i = XPT2046_AVG - 1; i > 0 ; i--)
  {
    avg_buf_x[i] = avg_buf_x[i - 1];
    avg_buf_y[i] = avg_buf_y[i - 1];
  }

  // Insert the new point
  avg_buf_x[0] = *x;
  avg_buf_y[0] = *y;

  if(avg_last < XPT2046_AVG)
  {
    avg_last++;
  }

  // Sum the x and y coordinates
  int32_t x_sum = 0;
  int32_t y_sum = 0;
  for(i = 0; i < avg_last ; i++)
  {
    x_sum += avg_buf_x[i];
    y_sum += avg_buf_y[i];
  }

  // Normalize the sums
  (*x) = (int32_t)x_sum / avg_last;
  (*y) = (int32_t)y_sum / avg_last;
}

static int16_t xpt2046_cmd(uint8_t cmd)
{
  uint8_t data[2] = {0x00, 0x00};
  int16_t val = 0;
  touch_read_data( cmd, data, 2 );
  val = (data[0] << 8) | data[1];
  return val;
}

static xpt2046_touch_detect_t xpt2048_is_touch_detected(void)
{
  xpt2046_touch_detect_t touch_detect = TOUCH_NOT_DETECTED;

  int16_t z1 = xpt2046_cmd(CMD_Z1_READ) >> 3;
  int16_t z2 = xpt2046_cmd(CMD_Z2_READ) >> 3;

  int16_t z = z1 + 4096 - z2;

  // printf("Z1 = %d, Z2 = %d, Z = %d\n", z1, z2, z);

  if( z > XPT2046_TOUCH_THRESHOLD )
  {
    touch_detect = TOUCH_DETECTED;
    // printf("touch detected\n");
  }
  return touch_detect;
}

static void xpt2046_corr(int16_t * x, int16_t * y)
{
#if XPT2046_XY_SWAP != 0
  int16_t swap_tmp;
  swap_tmp = *x;
  *x = *y;
  *y = swap_tmp;
#endif

  // Simple Line Equation y = m*x + c
  if( (*x) > XPT2046_X_MIN )
  {
    (*x) -= XPT2046_X_MIN;
  }
  else
  {
    (*x) = 0;
  }

  if( (*y) > XPT2046_Y_MIN )
  {
    (*y) -= XPT2046_Y_MIN;
  }
  else
  {
    (*y) = 0;
  }
  printf("Correlation Modified x = %d, y = %d\n", *x, *y);

  (*x) = (uint32_t)( (uint32_t)(*x) * (uint32_t)tft_get_width()) / (XPT2046_X_MAX - XPT2046_X_MIN);
  // (*x) = (uint32_t)( (uint32_t)(*x) * (uint32_t)tft_get_width());
  // *(x) = (uint32_t)( (uint32_t)(*x) / (XPT2046_X_MAX - XPT2046_X_MIN));

  (*y) = (uint32_t)( (uint32_t)(*y) * (uint32_t)tft_get_height()) / (XPT2046_Y_MAX - XPT2046_Y_MIN);
  // (*y) = (uint32_t)( (uint32_t)(*y) * (uint32_t)tft_get_height());
  // (*y) = (uint32_t)( (uint32_t)(*y) / (XPT2046_Y_MAX - XPT2046_Y_MIN));

  printf("Correlation Translated x = %d, y = %d\n", *x, *y);

#if XPT2046_X_INV != 0
  (*x) =  tft_get_width() - (*x);
#endif

#if XPT2046_Y_INV != 0
  (*y) =  tft_get_height() - (*y);
#endif
}
