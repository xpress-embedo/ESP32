#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN                  12
#define DHTTYPE                 DHT11

#define DHT_REFRESH_TIME        2000u   // 2 seconds

static uint32_t dht_refresh_timestamp = 0u;
static float temperature = 0.0;
static float humidity = 0.0;
DHT dht(DHTPIN, DHTTYPE);   /* DHT Instance, also initializes the DHT */

void setup() 
{
  delay(100);
  Serial.begin( 115200 );
  Serial.println("Hello World from ESP32");

  dht.begin();
  delay( 1000 );
}

void loop() 
{
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
    }
  }
}