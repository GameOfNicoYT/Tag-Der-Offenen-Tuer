#include <ArduinoJson.h>

#define JoyStickX 

StaticJsonDocument<200> doc;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  delay(500);

  doc["joystick"]["x"] = analogRead();
  
  serializeJson(doc, Serial);
  Serial.println();
}
