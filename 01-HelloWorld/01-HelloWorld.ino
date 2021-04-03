const int LED_BLUE = 0;
const int LED_RED = 2;
const int LED_GREEN = 4;

void setup()
{
  pinMode( LED_BLUE, OUTPUT);
  pinMode( LED_RED, OUTPUT);
  pinMode( LED_GREEN, OUTPUT);
  // Initialize Serial Communication
  Serial.begin(115200);
  delay(100);
  Serial.println("Hello World from ESP32");
}

void loop()
{
  // digitalWrite( LED_BLUE, HIGH);
  // digitalWrite( LED_RED, HIGH);
  digitalWrite( LED_GREEN, HIGH);
  Serial.println("Led Blue On");
  delay(1000);
  // digitalWrite( LED_BLUE, LOW);
  // digitalWrite( LED_RED, LOW);
  digitalWrite( LED_GREEN, LOW);
  Serial.println("Led Blue Off");
  delay(1000);
}
