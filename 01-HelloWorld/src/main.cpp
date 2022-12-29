#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Macros
#define DHTPIN                          (12)
#define DHTTYPE                         (DHT11)

#define DHT_REFRESH_TIME                (20000u)   // 20 seconds
#define MQTT_PUB_BUFF_SIZE              (20u)
#define WIFI_NOT_CONNECT_COUNT_MAX      (10)

// Private Variables
static uint32_t dht_refresh_timestamp = 0u;
static float temperature = 0.0;
static float humidity = 0.0;
DHT dht(DHTPIN, DHTTYPE);

// Update these Information
// const char* ssid = "TestWiFi";                  // WiFi Name
// const char* password = "12345678";              // WiFi Password
const char* ssid = "gigacube-D733";                     // WiFi Name
const char* password = "5yR8m2B6Y4egrJQ5";              // WiFi Password
// const char* ssid = "WLAN-214460";                       // WiFi Name
// const char* password = "5139300621152829";              // WiFi Password
const char* mqtt_server = "hairdresser.cloudmqtt.com";  // MQTT Server Name
const int mqtt_port = 17259;                            // MQTT Server Port
const char* user_name = "pyptiouq";                     // MQTT Server Instance User Name
const char* mqtt_pswd = "aQp113ENJeO9";                 // MQTT Server Instance Password

WiFiClient esp_client;
PubSubClient client( esp_client );
char mqtt_pub_msg[MQTT_PUB_BUFF_SIZE] = { 0 };

// private function prototypes
void setup_wifi( void );
void mqtt_callback( char* topic, byte* payload, uint16_t length );
void mqtt_reconnect( void );

void setup() 
{
  delay(100);
  Serial.begin( 115200 );
  Serial.println("Hello World from ESP32");

  dht.begin();
  delay( 1000 );
  
  // Setup ESP32 with WiFi
  setup_wifi();

  client.setServer( mqtt_server, mqtt_port );
  client.setCallback( mqtt_callback );
}

void loop()
{
  uint32_t now;
  now = millis();
  if( client.connected() == false )
  {
    mqtt_reconnect();
  }

  // DHT11 Refresh Timed Task
  if( now - dht_refresh_timestamp >= DHT_REFRESH_TIME )
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
      // publishing data to mqtt server
      snprintf( mqtt_pub_msg, MQTT_PUB_BUFF_SIZE, "%02d,%02d", \
                (uint8_t)(temperature), (uint8_t)(humidity) );
      // printing only for debugging purposes
      Serial.print("Publish Message : ");
      Serial.println( mqtt_pub_msg );
      client.publish("home", mqtt_pub_msg );
    }
  }
}

// private function definitions
void setup_wifi( void )
{
  uint16_t not_connected_counter = 0u;

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Station Mode
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1500);
    Serial.print(".");
    not_connected_counter++;
    if( not_connected_counter > WIFI_NOT_CONNECT_COUNT_MAX )
    {
      Serial.println("Resetting ESP32, as not able to connected to WiFi");
      ESP.restart();
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqtt_callback( char* topic, byte* payload, uint16_t length )
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (uint16_t i = 0; i < length; i++)
  {
    Serial.print((char)payload[i], HEX);
  }
  Serial.println();

  // Check which topic we have received and based on that take action
  // led topic
  if( strcmp(topic,"led") == 0 )
  {
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1') 
    {
      // TODO: XS to connect an actual led
      // digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on
    }
    else 
    {
      // TODO: XS to connect an actual led
      // digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off
    }
  }
}

void mqtt_reconnect( void )
{
  // stay here/loop untill connected again
  while( client.connected() == false )
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    // String clientId = "ESP8266Client-";
    // clientId += String(random(0xffff), HEX);
    String clientId = "EmbeddedLab";
    Serial.print("Client Id:  ");
    Serial.println(clientId);
    // Attempt to connect
    // if (client.connect(clientId.c_str())) 
    if ( client.connect(clientId.c_str(), user_name, mqtt_pswd) )
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // client.publish("Broadcast", "Connected with MQTT Server");
      // ... and resubscribe (Topic is "LED", to control the on board LED)
      client.subscribe("led/#");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // TODO: XS this crude delay will block other tasks
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}