/*
 * xpt2046.c
 *
 *  Created on: Jan 20, 2024
 *      Author: xpress_embedo
 */

#include "tft.h"
#include "xpt2046.h"

// Defines

#define CMD_X_READ                  0b10010000  // NOTE: XPT2046 data sheet says this is actually Y
#define CMD_Y_READ                  0b11010000  // NOTE: XPT2046 data sheet says this is actually X
#define CMD_Z1_READ                 0b10110000  // 0xB0
#define CMD_Z2_READ                 0b11000000  // 0xC0
#define XPT2046_TOUCH_THRESHOLD     400 // Threshold for touch detection

typedef enum {
  TOUCH_NOT_DETECTED = 0,
  TOUCH_DETECTED,
} xpt2046_touch_detect_t;

// Private Function Prototypes
static void xpt2046_avg(int16_t * x, int16_t * y);
static int16_t xpt2046_cmd(uint8_t cmd);
static xpt2046_touch_detect_t xpt2048_is_touch_detected(void);

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
    x = xpt2046_cmd(CMD_X_READ);
    y = xpt2046_cmd(CMD_Y_READ);
    printf("X = %d, Y = %d\n", x, y);
  }
}

// Private Function Definitions
static void xpt2046_avg(int16_t * x, int16_t * y)
{

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

  printf("Z1 = %d, Z2 = %d, Z = %d\n", z1, z2, z);

  if( z > XPT2046_TOUCH_THRESHOLD )
  {
    touch_detect = TOUCH_DETECTED;
    printf("touch detected\n");
  }
  return touch_detect;
}
