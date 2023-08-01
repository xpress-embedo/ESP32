#include "Wire.h"
#include "BluetoothSerial.h"
#include <MPU6050_light.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Private Objects and Variables
MPU6050 mpu(Wire);
BluetoothSerial SerialBT;
unsigned long timer = 0;

void setup() 
{
  Serial.begin(115200);
  // Bluetooth Device Name
  SerialBT.begin("ESP32-Bluetooth");
  Wire.begin();
  
  byte status = mpu.begin();
  
  while(status!=0){ } // stop everything if could not connect to MPU6050
  delay(1000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  mpu.calcOffsets(); // gyro and accelero
}

void loop() 
{
  mpu.update();
  if( (millis()-timer) > 1000 )
  {
    timer = millis();
    /*
  	Serial.print("X : ");
  	Serial.print(mpu.getAngleX());
  	Serial.print("\tY : ");
  	Serial.print(mpu.getAngleY());
  	Serial.print("\tZ : ");
  	Serial.println(mpu.getAngleZ());
    */
    // Printing Data as per our Qt PC Application format
    Serial.print(mpu.getAngleX());
    Serial.print(",");
    Serial.println(mpu.getAngleY());
    // Sending Data as per our Qt Android format
    SerialBT.print(mpu.getAngleX());
    SerialBT.print(",");
    SerialBT.println(mpu.getAngleY());
  }
}
