#include <lvgl.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */

/*Change to your screen resolution*/
static const uint32_t screenWidth  = 320;
static const uint32_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
/* Declare a buffer for 1/10 screen size */
static lv_color_t buf[ screenWidth * 10 ];

/* Style instance contains properties such as background color, border,
width, font etc. that describes the appearance of the objects 
Styles are represented with lv_style_t variables. Only their pointer 
is saved in the objects so they need to be defined as static or global.
Before using a style it needs to be initialized with 
lv_style_init(&style1). After that,properties can be added to configure
the style. */
static lv_style_t style1;

static const uint32_t LVGL_REFRESH_TIME = 5u; // In milliseconds
static uint32_t lvgl_refresh_timestamp = 0u;

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print( lv_log_level_t level, const char * file, uint32_t line, const char * fn_name, const char * dsc )
{
  Serial.printf( "%s(%s)@%d->%s\r\n", file, fn_name, line, dsc );
  Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
  uint32_t w = ( area->x2 - area->x1 + 1 );
  uint32_t h = ( area->y2 - area->y1 + 1 );
  
  tft.startWrite();
  tft.setAddrWindow( area->x1, area->y1, w, h );
  tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
  tft.endWrite();
  
  lv_disp_flush_ready( disp );
}

void setup()
{
  Serial.begin( 115200 ); /* prepare for possible serial debug */
  Serial.println( "Hello Arduino! (V8.0.X)" );
  Serial.println( "I am LVGL_Arduino" );

  lv_init();

#if LV_USE_LOG != 0
  lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif

  tft.begin();          /* TFT init */
  tft.setRotation( 1 ); /* Landscape orientation */

  /* Initialize the display buffer */
  lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * 10 );

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;    /* Descriptor of a display driver */
  lv_disp_drv_init( &disp_drv );    /* Basic Inialization */
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;   /* Set the horizonral resolution of the display */
  disp_drv.ver_res = screenHeight;  /* Set the vertical resolution of the display */
  disp_drv.flush_cb = my_disp_flush;/* driver function to flush the display */
  disp_drv.draw_buf = &draw_buf;    /* Assign the buffer to the display */
  lv_disp_drv_register( &disp_drv );/* Finally register the driver */

  /* Initialize the Styles */
  lv_style_init(&style1);
  lv_style_set_bg_color( &style1, lv_color_hex(0xa000000) );    // Black Color
  lv_style_set_text_color( &style1, lv_color_hex(0xFF0000) );   // Red Color
  /* Create simple labels 
  lv_scr_act() -- is used to get the current screen,
  and to load a screen use lv_scr_load(scr1)
  */
  lv_obj_t *label = lv_label_create( lv_scr_act() );
  // Add Style to Label is not working completely, only text color can be changed
  // background color changing is not working TODO: XE
  // lv_obj_add_style( label, &style1, LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_add_style( label, &style1, 0 );  // Above Line and this line means same
  lv_label_set_text( label, "Hello Arduino! (V8.0.X)" );
  lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );
  
  Serial.println( "Setup done" );
}

void loop()
{
  // LVGL Refresh Timed Task
  if( millis() - lvgl_refresh_timestamp >= LVGL_REFRESH_TIME )
  {
    lvgl_refresh_timestamp = millis();
    lv_timer_handler(); /* let the GUI do its work */
  }
}
