#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>

// Macros
#define LVGL_REFRESH_TIME                   (5u)      // 5 milliseconds

// Private Variables
// Screen Resolution
static const uint32_t screenWidth  = 320;
static const uint32_t screenHeight = 240;
// LVGL related stuff
static lv_disp_draw_buf_t draw_buf;
// Declare a buffer for 1/10 screen size
static lv_color_t buf[ screenWidth * 10 ];
// TFT Instance
TFT_eSPI tft = TFT_eSPI();
static uint32_t lvgl_refresh_timestamp = 0u;

// Private functions
#if LV_USE_LOG != 0
/* Serial debugging */
void LVGL_Print( const char * buffer );
#endif
static void Display_Flush(lv_disp_drv_t *disp, const lv_area_t *area, \
                          lv_color_t *color_p );
static void Touch_Read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data );
static void LVGL_TaskInit( void );
static void LVGL_TaskMng( void );
static void Display_Init( void );

void setup()
{
  Serial.begin(115200);

  LVGL_TaskInit();
  Display_Init();
}

void loop()
{
}

// Private Function Definitions
static void LVGL_TaskInit( void )
{
  lv_init();
  lvgl_refresh_timestamp = millis();
}

static void LVGL_TaskMng( void )
{
  uint32_t now = millis();
  // LVGL Refresh Timed Task
  if( (now - lvgl_refresh_timestamp) >= LVGL_REFRESH_TIME )
  {
    lvgl_refresh_timestamp = now;
    // let the GUI does work
    lv_timer_handler();
  }
}

static void Display_Init( void )
{
  #if LV_USE_LOG != 0
   // register print function for debugging
  lv_log_register_print_cb( LVGL_Print );
  #endif
  // Initialize TFT
  tft.begin();
  // Set Orientation to Landscape
  tft.setRotation(1);

  // Calibration Values for my Setup
  uint16_t calData[5] = { 393, 3484, 305, 3314, 7 };
  tft.setTouch(calData);

  // Initialize the display buffer
  lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * 10 );

  // Initialize the display
  static lv_disp_drv_t disp_drv;    /* Descriptor of a display driver */
  lv_disp_drv_init( &disp_drv );    /* Basic Inialization */
  /* Change the following line to your display resolution */
  disp_drv.hor_res = screenWidth;   /* Set the horizonral resolution of the display */
  disp_drv.ver_res = screenHeight;  /* Set the vertical resolution of the display */
  disp_drv.flush_cb = Display_Flush;/* driver function to flush the display */
  disp_drv.draw_buf = &draw_buf;    /* Assign the buffer to the display */
  lv_disp_drv_register( &disp_drv );/* Finally register the driver */

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init( &indev_drv );
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = Touch_Read;
  lv_indev_drv_register( &indev_drv );
}

static void Touch_Read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
  uint16_t touchX, touchY;

  bool touched = tft.getTouch( &touchX, &touchY, 600 );

  if( !touched )
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    data->state = LV_INDEV_STATE_PR;
    /*Set the coordinates*/
    data->point.x = touchX;
    data->point.y = touchY;
  }
}

static void Display_Flush(  lv_disp_drv_t *disp, const lv_area_t *area, \
                            lv_color_t *color_p )
{
  uint32_t w = ( area->x2 - area->x1 + 1 );
  uint32_t h = ( area->y2 - area->y1 + 1 );
  
  tft.startWrite();
  tft.setAddrWindow( area->x1, area->y1, w, h );
  tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
  tft.endWrite();
  
  lv_disp_flush_ready( disp );
}

#if LV_USE_LOG != 0
/* Serial debugging */
void LVGL_Print( const char * buffer )
{
  Serial.printf( buffer );
  Serial.flush();
}
#endif