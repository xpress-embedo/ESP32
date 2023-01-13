/*
 * Display Manager.c
 *
 *  Created on: 01-Nov-2022
 *      Author: xpress_embedo
 */
#include <lvgl.h>
#include <TFT_eSPI.h>

#include "display_mng.h"
#include "ui.h"
#include "main.h"

/************************************Macros************************************/
#define DISP_STATE_INIT_WAIT_TIME                     (2000u)


typedef enum _Display_States_e
{
  DISP_STATE_INIT = 0,            // Configure Station Mode and Disconnect
  DISP_STATE_INIT_WAIT,           // Wait for sometime after initializing WiFi
  DISP_STATE_SCAN_SSID,           // Let the list of WiFi SSID's available
  DISP_STATE_CONNECT_MENU,        // Show UI generated to connect to Router
  DISP_STATE_MAX
} Display_State_e;

/*---------------------------Private Variables--------------------------------*/
// Screen Resolution
static const uint32_t screenWidth  = 320;
static const uint32_t screenHeight = 240;
// LVGL related stuff
static lv_disp_draw_buf_t draw_buf;
// Declare a buffer for 1/10 screen size
static lv_color_t buf[ screenWidth * 10 ];
// TFT Instance
TFT_eSPI tft = TFT_eSPI();

// Display State to track what to display
static Display_State_e disp_state = DISP_STATE_INIT;
static uint32_t display_timestamp = 0u;

/*--------------------------Private Function Prototypes-----------------------*/
#if LV_USE_LOG != 0
/* Serial debugging */
void LVGL_Print( const char * buffer );
#endif
static void Display_Flush(lv_disp_drv_t *disp, const lv_area_t *area, \
                          lv_color_t *color_p );
static void Touch_Read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data );

// display state machine related functions
static void Display_StateInit();

static void Display_StateInit()
{
  static lv_obj_t *power_up_bar = lv_bar_create(lv_scr_act());
  lv_obj_set_size( power_up_bar, LV_PCT(100), LV_PCT(10) );
  lv_obj_align(power_up_bar, LV_ALIGN_BOTTOM_MID, 0, -20);
  lv_bar_set_range( power_up_bar, 0, 100 );
  lv_bar_set_value( power_up_bar, 10, LV_ANIM_OFF );
  // make sure no delay after configuring station mode
  WiFi_Init();
}


/*---------------------------Public Function Definitions----------------------*/
void Display_Init( void )
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

  // ui_init();

  // // update drop down list
  // lv_dropdown_set_options( ui_DropDownSSID, Get_WiFiSSID_DD_List() );

  // // Hide the keyboard at power-up can be done by adding the hidden flag
  // lv_obj_add_flag( ui_Keyboard, LV_OBJ_FLAG_HIDDEN );
}

/**
 * @brief This function handles all the display related stuff, this function is
 *        called continously inside the super loop, and all timings related 
 *        stuff is handled here itself.
 * @param  none
 */
void Display_Mng( void )
{
  uint32_t now = millis();
  switch( disp_state )
  {
    case DISP_STATE_INIT:
      Display_StateInit();
      disp_state = DISP_STATE_INIT_WAIT;
    break;
    case DISP_STATE_INIT_WAIT:
      if( (now - display_timestamp) >= DISP_STATE_INIT_WAIT_TIME )
      {
        display_timestamp = now;
        // disp_state = DISP_STATE_SCAN_SSID;
      }
    break;
    case DISP_STATE_SCAN_SSID:
      // This is blocking function, an can take upto 5 seconds
      WiFi_ScanSSID();
    break;
    case DISP_STATE_CONNECT_MENU:
    break;
    case DISP_STATE_MAX:
    default:
      disp_state = DISP_STATE_INIT;
    break;
  };
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
    // uncomment to debug touch points
    // Serial.print( "Data x " );
    // Serial.println( touchX );
    // Serial.print( "Data y " );
    // Serial.println( touchY );
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