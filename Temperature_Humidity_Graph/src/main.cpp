#include "main.h"
#include <lvgl.h>
#include <DHT.h>
#include "display_mng.h"

// Macros
#define DHT_PIN                             (12)
#define DHT_TYPE                            (DHT11)
#define LVGL_REFRESH_TIME                   (5u)      // 5 milliseconds
#define DHT_REFRESH_TIME                    (2000u)   // 2 seconds


// Private Variables
static uint32_t lvgl_refresh_timestamp = 0u;
static uint32_t dht_refresh_timestamp = 0u;

// DHT Related Variables and Instances
DHT dht(DHT_PIN, DHT_TYPE);
static Sensor_Data_s sensor_data = 
{
  .sensor_idx = 0u,
};

// Private functions
static void DHT_TaskInit( void );
static void DHT_TaskMng( void );
static void LVGL_TaskInit( void );
static void LVGL_TaskMng( void );

void setup()
{
  delay(1000);
  Serial.begin(115200);
  Serial.println("Temperature and Humdity Graph Using LVGL");

  // Intialize the LVGL Library
  LVGL_TaskInit();
  
  // Initialize Display and Display Buffers
  Display_Init();

  DHT_TaskInit();
}

void loop()
{
  DHT_TaskMng();
  Display_Mng();
  LVGL_TaskMng();
}

Sensor_Data_s * Get_TemperatureAndHumidity( void )
{
  return &sensor_data;
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
  float temperature = 0.0;
  float humidity = 0.0;
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
      if( sensor_data.sensor_idx < SENSOR_BUFF_SIZE )
      {
        sensor_data.temperature[sensor_data.sensor_idx] = (uint8_t)(temperature);
        sensor_data.humidity[sensor_data.sensor_idx] = (uint8_t)(humidity);
        sensor_data.sensor_idx++;
        // Reset to Zero
        if( sensor_data.sensor_idx >= SENSOR_BUFF_SIZE )
        {
          sensor_data.sensor_idx = 0u;
        }
      }
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
