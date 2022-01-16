#include <lvgl.h>
#include <TFT_eSPI.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN      12
#define DHTTYPE     DHT11

#define LVGL_REFRESH_TIME       5u      // 5 ms
#define DHT_REFRESH_TIME        2000u   // 2 seconds

static uint32_t lvgl_refresh_timestamp = 0u;
static uint32_t dht_refresh_timestamp = 0u;
static float temperature = 0.0;
static float humidity = 0.0;

TFT_eSPI tft = TFT_eSPI();  /* TFT instance */
DHT dht(DHTPIN, DHTTYPE);   /* DHT Instance, also initializes the DHT */

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
  delay(100);
  Serial.begin( 115200 ); /* prepare for possible serial debug */
  Serial.println( "Hello Arduino! (V8.0.X)" );
  Serial.println( "I am using LVGL_Arduino" );

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
  
  /* Create simple labels lv_scr_act() -- is used to get the current screen,
  and to load a screen use lv_scr_load(scr1) */
  static lv_obj_t *label = lv_label_create( lv_scr_act() );
  
  // Add Style to Label is not working completely, only text color can be changed
  // background color changing is not working TODO: XE
  // lv_obj_add_style( label, &style1, LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_add_style( label, &style1, 0 );  // Above Line and this line means same
  lv_label_set_text( label, "Hello Arduino! (V8.0.X)" );
  lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );

  dht.begin();
  delay(1000);
  // Clear the whole Screen
  lv_obj_clean(lv_scr_act());
}

void loop()
{
  static lv_obj_t *lbl_temperature = lv_label_create( lv_scr_act() );
  static lv_obj_t *lbl_humidity = lv_label_create( lv_scr_act() );
  
  // LVGL Refresh Timed Task
  if( millis() - lvgl_refresh_timestamp >= LVGL_REFRESH_TIME )
  {
    lvgl_refresh_timestamp = millis();
    lv_timer_handler(); /* let the GUI do its work */
  }

  // DHT11 Refresh Timed Task
  if( millis() - dht_refresh_timestamp >= DHT_REFRESH_TIME )
  {
    dht_refresh_timestamp = millis();
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    humidity = dht.readHumidity();
    // Read temperature as Celsius (the default)
    temperature = dht.readTemperature();
    
    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperature) ) 
    {
      Serial.println(F("Failed to read from DHT sensor!"));
    }
    else
    {
      Serial.print(F("Humidity: "));
      Serial.print(humidity);
      Serial.print(F("%  Temperature: "));
      Serial.print(temperature);
      Serial.println(F("°C "));
      // Set LV_SPRINTF_USE_FLOAT in lcd_conf.h file
      lv_label_set_text_fmt(lbl_temperature, "Temperature: %f", temperature);
    }
  }
  
}
