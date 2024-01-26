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

// These are the values at lowest coordinates and maximum coordinates
// These values are used to map the touch screen over display
// These values can also be considered as calibration, a run-time calibration
// is more meaningful but as of now it is hard-coded
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

/**
 * @brief Initialize the Touch Screen drivers and other related stuff
 *        Right now it is not used as drivers initialization will be done together
 *        with the display initialization, in future here we can configure the
 *        touch detection interrupt.
 */
void xpt2046_init(void)
{

}

/**
 * @brief This funcion returns the detected touch events and returns the values 
 *        of x and y coordinates after mapping and averaging.
 * @param det_x pointer to x coordinate
 * @param det_y pointer to y coordinate
 * @return true when touch is detected else false
 */
uint8_t xpt2046_read(int16_t *det_x, int16_t *det_y)
{
  static int16_t last_x = 0;
  static int16_t last_y = 0;
  uint8_t valid = false;

  int16_t x = last_x;
  int16_t y = last_y;

  if( xpt2048_is_touch_detected() == TOUCH_DETECTED )
  {
    valid = true;
    xpt2046_cmd(CMD_X_READ);
    x = xpt2046_cmd(CMD_X_READ);
    y = xpt2046_cmd(CMD_Y_READ);
    // printf("\nX = %d, Y = %d\n", x, y);

    // Normalize Data back to 12-bits
    x = x >> 4;
    y = y >> 4;
    // printf("Normalize X = %d, Y = %d\n", x, y);

    xpt2046_corr(&x, &y);
    xpt2046_avg(&x, &y);

    last_x = x;
    last_y = y;
    *det_x = x;
    *det_y = y;
    // printf("Mapped X = %d, Y = %d\n\n", x, y);
  }
  else
  {
    avg_last = 0;
  }
  return valid;
}

// Private Function Definitions

/**
 * @brief This function performs the averaging on the samples.
 *        Latest entry is inserted at the first index while previous entries are
 *        shifted, this helps to reduce the noise and unintentional touches
 * @param x pointer to data containing x coordinates
 * @param y pointer to data containing y coordinates
 */
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

/**
 * @brief The function is used to send command to the touch controller.
 *        The function "touch_read_data" is implemented in other module to keep
 *        this file generic.
 * @param cmd Command to be transmitted to XPT2046
 * @return the values read from the touch controller
 * @note   The data is 16-bit while SPI returns the 8-bit data two times, hence
 *         we shift the data[0] by 8 times and then or it with data[1] to make
 *         2 8-bits data back into 16-bit data.
 */
static int16_t xpt2046_cmd(uint8_t cmd)
{
  uint8_t data[2] = {0x00, 0x00};
  int16_t val = 0;
  touch_read_data( cmd, data, 2 );
  val = (data[0] << 8) | data[1];
  return val;
}

/**
 * @brief The function detects if touch is pressed somewhere or not.
 *        This function works by reading the Z parameters which is also known as
 *        pressure parameter, when pressure is greater than XPT2046_TOUCH_THRESHOLD
 *        touch is detected.
 * @param  none
 * @return TOUCH_DETECTED if touch is detected else TOUCH_NOT_DETECTED
 */
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

/*
 * Explanation about this function
 * This function uses simple linear equation formula to calculated the mapped value.
 * We checked manually by pressing at (0,0) of the touch screen, and let's say
 * we get the value (XPT2046_X_MIN, XPT2046_Y_MIN) and then we touched at the
 * point (max, max) which in our case is (320, 240) but could be different also
 * and then we recorded the points (XPT2046_X_MAX, XPT2046_Y_MAX).
 * What we want when we receive XPT2046_X_MIN we should map it to 0 i.e. minimum
 * & when we received XPT2046_X_MAX we should map it to 320 i.e, maximum or tft width
 * Here we will use the simple Liner Equation formula y = m*x + c
 * where y is the output value for a given x (not to be confused with the x, y coordinates)
 * m = (y2-y1)/(x2-x1)
 * so our m = (tft_width - 0)/(x_max - x_min)
 * Now we have the value of slope, we can use the formula to the find the y intercept i.e. c
 * y1 = m*x1 + c
 * using y1 = 0 which is the value when x1 = x_min
 * 0 = ((tft_width - 0)/(x_max-x_min))*x_min + c
 * c = 0 - ((tft_width - 0)/(x_max-x_min))*x_min
 * c = ((tft_width)/(x_max-x_min))*x_min
 * So now complete formula is to calculate the mapped value when a x is received is:
 * y = m*x + c
 * y = (tft_width)*x/(x_max - x_min) - ((tft_width)/(x_max-x_min))*x_min
 * y = (tft_width/(x_max - x_min) (x - x_x_min)
 * And if you see below, we are using the same formula, only thing is divided it
 * into two steps x = x - x_min is first step and (tft_width/(x_max - x_min) is
 * second step.
 * Same thing is done for calculating the mapped y-coordinates
 * NOTE: I am again writing here, don't get confused with x and y with x and y
 * coordinates. In the above formula y means mapped value for input x.
 */

/**
 * @brief This function is to map the received coordinates as per display size.
 *        A detailed explainaton how this is done is explained above in comments.
 * @param x pointer to x data
 * @param y pointer to y data
 */
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
  // printf("Correlation Modified x = %d, y = %d\n", *x, *y);


  // Map X-Position to screen width
  (*x) = (uint32_t)( (uint32_t)(*x) * (uint32_t)tft_get_width()) / (XPT2046_X_MAX - XPT2046_X_MIN);

  // Map Y-Position to screen height
  (*y) = (uint32_t)( (uint32_t)(*y) * (uint32_t)tft_get_height()) / (XPT2046_Y_MAX - XPT2046_Y_MIN);

  // printf("Correlation Translated x = %d, y = %d\n", *x, *y);

#if XPT2046_X_INV != 0
  (*x) =  tft_get_width() - (*x);
#endif

#if XPT2046_Y_INV != 0
  (*y) =  tft_get_height() - (*y);
#endif
}
