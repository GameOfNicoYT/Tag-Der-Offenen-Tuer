#include <ArduinoJson.h>

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  if (Serial.available())
  {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, Serial);

    if (!error)
    {
      const char *sensor = doc["sensor"];
      long time = doc["time"];

      Serial.println(sensor);
    }
  }
}
