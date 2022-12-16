#include <Arduino.h>

static uint32_t idx = 0u;
void setup() 
{
  delay(100);
  Serial.begin( 9600 );
  Serial.println("Hello World from ESP32");
}

void loop() 
{
  Serial.print("Index Value = ");
  Serial.println(idx, DEC);
  idx++;
  delay(1000);
}