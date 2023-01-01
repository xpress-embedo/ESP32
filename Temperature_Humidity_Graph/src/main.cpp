#include "main.h"
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <DHT.h>

// Macros
#define DHT_PIN                             (12)
#define DHT_TYPE                            (DHT11)
#define LVGL_REFRESH_TIME                   (5u)      // 5 milliseconds
#define DHT_REFRESH_TIME                    (2000u)   // 2 seconds

// Private Variables
static uint32_t lvgl_refresh_timestamp = 0u;
static uint32_t dht_refresh_timestamp = 0u;

// Screen Resolution
static const uint32_t screenWidth  = 320;
static const uint32_t screenHeight = 240;

// LVGL related stuff
static lv_disp_draw_buf_t draw_buf;
// Declare a buffer for 1/10 screen size
static lv_color_t buf[ screenWidth * 10 ];

/* Style instance contains properties such as background color, border, width, 
font etc. that describes the appearance of the objects.
Styles are represented with lv_style_t variables. Only their pointer is saved in
the objects so they need to be defined as static or global. Before using a style
it needs to be initialized with lv_style_init(&style1). After that, properties 
can be added to configure the style. */
static lv_style_t style1;

// TFT Instance
TFT_eSPI tft = TFT_eSPI();

// DHT Related Variables and Instances
static float temperature = 0.0;
static float humidity = 0.0;
DHT dht(DHT_PIN, DHT_TYPE);
static uint8_t temp_sensor_1sec[260] = { 0 };   // 320-60
static uint16_t temp_sensor_1sec_idx = 0u;

// Private functions
static void Disp_Init( void );
static void DHT_TaskInit( void );
static void DHT_TaskMng( void );
static void LVGL_TaskInit( void );
static void LVGL_TaskMng( void );
static void Disp_Flush( lv_disp_drv_t *disp, const lv_area_t *area, \
                        lv_color_t *color_p );
#if LV_USE_LOG != 0
/* Serial debugging */
void LVGL_Print(lv_log_level_t level, const char * file, uint32_t line, \
                const char * fn_name, const char * dsc )
#endif

void setup()
{
  delay(1000);
  Serial.begin(115200);
  Serial.println("Temperature and Humdity Graph Using LVGL");

  // Intialize the LVGL Library
  LVGL_TaskInit();
  
  // Initialize Display and Display Buffers
  Disp_Init();

  DHT_TaskInit();
  //
  /* Create simple labels lv_scr_act() -- is used to get the current screen,
  and to load a screen use lv_scr_load(scr1) */
  static lv_obj_t *label = lv_label_create( lv_scr_act() );
  
  // Add Style to Label is not working completely, only text color can be changed
  // background color changing is not working TODO: XE
  // lv_obj_add_style( label, &style1, LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_add_style( label, &style1, 0 );  // Above Line and this line means same
  lv_label_set_text( label, "Temperature and Humidity" );
  lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );
}

void loop()
{
  DHT_TaskMng();
  LVGL_TaskMng();
}

uint8_t * Display_GetTempData( void )
{
  return temp_sensor_1sec;
}

// Private Function Definition
static void DHT_TaskInit( void )
{
  dht.begin();
  dht_refresh_timestamp = millis();
}

static void DHT_TaskMng( void )
{
  uint32_t now = millis();
  if( (now - dht_refresh_timestamp) >= DHT_REFRESH_TIME )
  {
    dht_refresh_timestamp = now;
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
      // lv_label_set_text_fmt(lbl_temperature, "Temperature: %f", temperature);
    }
  }
}

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

static void Disp_Init( void )
{
  #if LV_USE_LOG != 0
   // register print function for debugging
  lv_log_register_print_cb( my_print );
  #endif
  // Initialize TFT
  tft.begin();
  // Set Orientation to Landscape
  tft.setRotation(1);

  // Initialize the display buffer
  lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * 10 );

  // Initialize the display
  static lv_disp_drv_t disp_drv;    /* Descriptor of a display driver */
  lv_disp_drv_init( &disp_drv );    /* Basic Inialization */
  /* Change the following line to your display resolution */
  disp_drv.hor_res = screenWidth;   /* Set the horizonral resolution of the display */
  disp_drv.ver_res = screenHeight;  /* Set the vertical resolution of the display */
  disp_drv.flush_cb = Disp_Flush;   /* driver function to flush the display */
  disp_drv.draw_buf = &draw_buf;    /* Assign the buffer to the display */
  lv_disp_drv_register( &disp_drv );/* Finally register the driver */

  // Initialize the Styles
  lv_style_init(&style1);
  lv_style_set_bg_color( &style1, lv_color_hex(0xa000000) );    // Black Color
  lv_style_set_text_color( &style1, lv_color_hex(0xFF0000) );   // Red Color
}

static void Disp_Flush(  lv_disp_drv_t *disp, const lv_area_t *area, \
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
void LVGL_Print(  lv_log_level_t level, const char * file, uint32_t line, \
                  const char * fn_name, const char * dsc )
{
  Serial.printf( "%s(%s)@%d->%s\r\n", file, fn_name, line, dsc );
  Serial.flush();
}
#endif