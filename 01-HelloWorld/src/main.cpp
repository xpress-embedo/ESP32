#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Macros
#define DHTPIN                  (12)
#define DHTTYPE                 (DHT11)

#define DHT_REFRESH_TIME        (2000u)   // 2 seconds

// Private Variables
static uint32_t dht_refresh_timestamp = 0u;
static float temperature = 0.0;
static float humidity = 0.0;
DHT dht(DHTPIN, DHTTYPE);

// Update these Information
const char* ssid = "TestWiFi";                  // WiFi Name
const char* password = "12345678";              // WiFi Password
const char* mqtt_server = "m14.cloudmqtt.com";  // MQTT Server Name
const int mqtt_port = 18410;                    // MQTT Server Port
const char* user_name = "setsmjwc";             // MQTT Server Instance User Name
const char* mqtt_pswd = "apDnKqHRgAjA";         // MQTT Server Instance Password

WiFiClient esp_client;
PubSubClient client( esp_client );

// private function prototypes
void setup_wifi( void );

void setup() 
{
  delay(100);
  Serial.begin( 115200 );
  Serial.println("Hello World from ESP32");

  dht.begin();
  delay( 1000 );
  
  // Setup ESP32 with WiFi
  setup_wifi();
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

// private function definitions
void setup_wifi( void )
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}