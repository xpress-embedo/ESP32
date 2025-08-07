#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> //This library you can add via Include Library > Manage Library > 
LiquidCrystal_I2C lcd(0x27, 16,2);
#define led1_g 13 
#define led1_y 12
#define led1_r 11

#define led2_g 10
#define led2_y 9
#define led2_r 8

#define led3_g 7
#define led3_y 6
#define led3_r 5

#define led4_g 4
#define led4_y 3
#define led4_r 2

#define s1 A0
#define s2 A1
#define s3 A2
#define s4 A3

String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete
int swt1=0,swt2=0,swt3=0,swt4=0;
void setup() 
{
  //initialize serial:
  Serial.begin(115200);
  //reserve 200 bytes for the inputString:
  inputString.reserve(200);
  lcd.init();   // initializing the LCD
  lcd.backlight(); // Enable or Turn On the backlight
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waiting..... ");
  lcd.setCursor(0, 1);
  lcd.print("----GO SLOW----");
  pinMode(s1,INPUT);
  pinMode(s2,INPUT);
  pinMode(s3,INPUT);
  pinMode(s4,INPUT);
  pinMode(led1_g,OUTPUT);
  pinMode(led1_y,OUTPUT);
  pinMode(led1_r,OUTPUT);

  pinMode(led2_g,OUTPUT);
  pinMode(led2_y,OUTPUT);
  pinMode(led2_r,OUTPUT);

  pinMode(led3_g,OUTPUT);
  pinMode(led3_y,OUTPUT);
  pinMode(led3_r,OUTPUT);

  pinMode(led4_g,OUTPUT);
  pinMode(led4_y,OUTPUT);
  pinMode(led4_r,OUTPUT);
}

void loop() 
{
 swt1=digitalRead(s1);
 swt2=digitalRead(s2);
 swt3=digitalRead(s3);
 swt4=digitalRead(s4);
 if(swt1==HIGH)
 {
   lcd.setCursor(0, 0);
   lcd.print("SIDE 1 GREEN ");
   digitalWrite(led1_g,HIGH);
   digitalWrite(led1_y,LOW);
   digitalWrite(led1_r,LOW);        
   digitalWrite(led2_g,LOW);
   digitalWrite(led2_y,LOW);
   digitalWrite(led2_r,HIGH);  
   digitalWrite(led3_g,LOW);
   digitalWrite(led3_y,LOW);
   digitalWrite(led3_r,HIGH);        
   digitalWrite(led4_g,LOW);
   digitalWrite(led4_y,LOW);
   digitalWrite(led4_r,HIGH); 
   delay(5000);
 }
 else if(swt2==HIGH)
 {
   lcd.setCursor(0, 0);
   lcd.print("SIDE 2 GREEN ");
   digitalWrite(led1_g,LOW);
   digitalWrite(led1_y,LOW);
   digitalWrite(led1_r,HIGH);        
   digitalWrite(led2_g,HIGH);
   digitalWrite(led2_y,LOW);
   digitalWrite(led2_r,LOW);  
   digitalWrite(led3_g,LOW);
   digitalWrite(led3_y,LOW);
   digitalWrite(led3_r,HIGH);        
   digitalWrite(led4_g,LOW);
   digitalWrite(led4_y,LOW);
   digitalWrite(led4_r,HIGH); 
   delay(5000);
 }
 else if(swt3==HIGH)
 {
   lcd.setCursor(0, 0);
   lcd.print("SIDE 3 GREEN ");
   digitalWrite(led1_g,LOW);
   digitalWrite(led1_y,LOW);
   digitalWrite(led1_r,HIGH); 
          
   digitalWrite(led2_g,LOW);
   digitalWrite(led2_y,LOW);
   digitalWrite(led2_r,HIGH); 
    
   digitalWrite(led3_g,HIGH);
   digitalWrite(led3_y,LOW);
   digitalWrite(led3_r,LOW); 
          
   digitalWrite(led4_g,LOW);
   digitalWrite(led4_y,LOW);
   digitalWrite(led4_r,HIGH); 
   delay(5000);
 }
 else if(swt4==HIGH)
 {
   lcd.setCursor(0, 0);
   lcd.print("SIDE 4 GREEN ");
   digitalWrite(led1_g,LOW);
   digitalWrite(led1_y,LOW);
   digitalWrite(led1_r,HIGH);  
         
   digitalWrite(led2_g,LOW);
   digitalWrite(led2_y,LOW);
   digitalWrite(led2_r,HIGH);
     
   digitalWrite(led3_g,LOW);
   digitalWrite(led3_y,LOW);
   digitalWrite(led3_r,HIGH); 
          
   digitalWrite(led4_g,HIGH);
   digitalWrite(led4_y,LOW);
   digitalWrite(led4_r,LOW); 
   delay(5000);
 }
 else
 {
  while (Serial.available() > 0) 
  {
   char inChar = (char)Serial.read();
   if(inChar == '\n') 
   {
    stringComplete = true;
   }
   else
   {
    inputString += inChar;
   }
  }

  if(stringComplete)
  {
    Serial.println(inputString);
    if(inputString.indexOf("RED1") != -1)
    {
      digitalWrite(led1_g,LOW);
      digitalWrite(led1_y,LOW);
      digitalWrite(led1_r,HIGH);
    }
    if(inputString.indexOf("RED2") != -1)
    {
      digitalWrite(led2_g,LOW);
      digitalWrite(led2_y,LOW);
      digitalWrite(led2_r,HIGH);
    }
    if(inputString.indexOf("RED3") != -1)
    {
      digitalWrite(led3_g,LOW);
      digitalWrite(led3_y,LOW);
      digitalWrite(led3_r,HIGH);
     }

    if(inputString.indexOf("RED4") != -1)
     {
       digitalWrite(led4_g,LOW);
       digitalWrite(led4_y,LOW);
       digitalWrite(led4_r,HIGH);
     }

    if(inputString.indexOf("GREEN1") != -1)
    {
      lcd.setCursor(0, 0);
      lcd.print("SIDE 1 GREEN ");
       digitalWrite(led1_g,HIGH);
       digitalWrite(led1_y,LOW);
       digitalWrite(led1_r,LOW);
    }
    
    if(inputString.indexOf("GREEN2") != -1)
    {
        lcd.setCursor(0, 0);
        lcd.print("SIDE 2 GREEN ");
        digitalWrite(led2_g,HIGH);
        digitalWrite(led2_y,LOW);
        digitalWrite(led2_r,LOW);
    }

   if(inputString.indexOf("GREEN3") != -1)
   {
        lcd.setCursor(0, 0);
        lcd.print("SIDE 3 GREEN ");
        digitalWrite(led3_g,HIGH);
        digitalWrite(led3_y,LOW);
        digitalWrite(led3_r,LOW);
    }
    
   if(inputString.indexOf("GREEN4") != -1)
   {
        lcd.setCursor(0, 0);
        lcd.print("SIDE 4 GREEN ");
        digitalWrite(led4_g,HIGH);
        digitalWrite(led4_y,LOW);
        digitalWrite(led4_r,LOW);
    }


    if(inputString.indexOf("YELLOW1") != -1)
    {
        digitalWrite(led1_g,LOW);
        digitalWrite(led1_y,HIGH);
        digitalWrite(led1_r,LOW);
    }
    if(inputString.indexOf("YELLOW2") != -1)
    {
      digitalWrite(led2_g,LOW);
      digitalWrite(led2_y,HIGH);
      digitalWrite(led2_r,LOW);
    }

    if(inputString.indexOf("YELLOW3") != -1)
    {
      digitalWrite(led3_g,LOW);
      digitalWrite(led3_y,HIGH);
      digitalWrite(led3_r,LOW);
    }

    if(inputString.indexOf("YELLOW4") != -1)
    {
      digitalWrite(led4_g,LOW);
      digitalWrite(led4_y,HIGH);
      digitalWrite(led4_r,LOW);
    }

    if(inputString.indexOf("OFFALL") != -1)
    {
     digitalWrite(led1_g,LOW);
     digitalWrite(led1_y,LOW);
     digitalWrite(led1_r,LOW);
     digitalWrite(led2_g,LOW);
     digitalWrite(led2_y,LOW);
     digitalWrite(led2_r,LOW);

     digitalWrite(led3_g,LOW);
     digitalWrite(led3_y,LOW);
     digitalWrite(led3_r,LOW);
     digitalWrite(led4_g,LOW);
     digitalWrite(led4_y,LOW);
     digitalWrite(led4_r,LOW);
    }
    
    stringComplete = false;
    inputString = "";
  }     
 }
}
