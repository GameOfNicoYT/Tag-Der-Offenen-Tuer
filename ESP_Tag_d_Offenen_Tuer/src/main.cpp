/*
 Copyright (c) 2023 Nico Proyer, nicoproyer.at
 Permission is hereby granted, free of charge,
 to any person obtaining a copy of this software
 and associated documentation files (the “Software”),
 to deal in the Software without restriction, including
 without limitation the rights to use, copy,
 modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit
 persons to whom the Software is furnished to do so,
 subject to the following conditions:The above copyright
 notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF
 ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
 SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 You should have received a copy of the MIT copyright license
 along with this program.  If not, see <https://opensource.org/license/mit/>.


 Second Copyright holder:
 Copyright (C) 2020 chester4444@wolke7.net
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <WiFi.h>
#include <mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include "WebSocketService.h"
#include "esp_wpa2.h"
#include "credentials.h"

#define LED_STATUS 4
#define OTA_NAME "webserver" // defined in platformio.ini

AsyncWebServer webServer(80);
WebSocketService webSocketService;

uint8_t wifiConnectCounter = 0;

bool ConnectWifi(void)
{
  int i = 0;
  bool isWifiValid = false;

  Serial.println("starting scan");
  // scan for nearby networks:
  int numSsid = WiFi.scanNetworks();

  Serial.print("scanning WIFI, found ");
  Serial.print(numSsid);
  Serial.println(" available access points:");

  if (numSsid == -1)
  {
    Serial.println("Couldn't get a wifi connection");
    return false;
  }

  for (int i = 0; i < numSsid; i++)
  {
    Serial.print(i + 1);
    Serial.print(". ");
    Serial.print(WiFi.SSID(i));
    Serial.print("  ");
    Serial.println(WiFi.RSSI(i));
  }

  // search for given credentials
  for (CREDENTIAL credential : credentials)
  {
    for (int j = 0; j < numSsid; ++j)
    {
      if (strcmp(WiFi.SSID(j).c_str(), credential.ssid) == 0)
      {
        Serial.print("credentials found for: ");
        Serial.println(credential.ssid);
        currentWifi = credential;
        isWifiValid = true;
      }
    }
  }

  if (!isWifiValid)
  {
    Serial.println("no matching credentials");
    return false;
  }

  // try to connect
  Serial.println(WiFi.macAddress());

  if (strlen(currentWifi.username))
  {
    // username not empty -> WPA2  magic starts here
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)currentWifi.username, strlen(currentWifi.username));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)currentWifi.username, strlen(currentWifi.username));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)currentWifi.password, strlen(currentWifi.password));
    // esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();
    esp_wifi_sta_wpa2_ent_enable();
    //  WPA2 enterprise magic ends here

    WiFi.begin(currentWifi.ssid);
  }
  else
  {
    // try to connect WPA
    WiFi.begin(currentWifi.ssid, currentWifi.password);
    WiFi.setHostname(OTA_NAME);
    Serial.println("");
    Serial.print("Connecting to WiFi ");
    Serial.println(currentWifi.ssid);
  }

  i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(LED_STATUS, LOW);
    delay(300);
    Serial.print(".");
    digitalWrite(LED_STATUS, HIGH);
    delay(300);
    if (i++ > 50)
    {
      // giving up
      ESP.restart();
      return false;
    }
  }
  return true;
}

void setupOTA()
{
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(OTA_NAME);

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]()
                     {
    
    Serial.print(F("Start updating "));
    if (ArduinoOTA.getCommand() == U_FLASH) {
      Serial.println(F("sketch"));
    } else { // U_FS
      Serial.println(F("filesystem"));
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    LittleFS.end(); });

  ArduinoOTA.onEnd([]()
                   { Serial.println(F("\nEnd")); });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    digitalWrite(LED_STATUS, !digitalRead(LED_STATUS)); });

  ArduinoOTA.onError([](ota_error_t error)
                     {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println(F("Auth Failed"));
    else if (error == OTA_BEGIN_ERROR) Serial.println(F("Begin Failed"));
    else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect Failed"));
    else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive Failed"));
    else if (error == OTA_END_ERROR) Serial.println(F("End Failed")); });

  ArduinoOTA.begin();
}

void setupFilesystem()
{
  if (LittleFS.begin())
  {
    Serial.println("FileSystem started");
  }
  else
  {
    Serial.println("formatting FileSystem ...");
    LittleFS.format();
    ESP.restart();
  }
}

void setup()
{
  // pinMode(LED_STATUS, OUTPUT);

  Serial.begin(9600);
  Serial2.begin(115200);

  Serial.println("\nStarting WebServer - (" OTA_NAME ")");
  setupFilesystem();
}

enum ControlStateType
{
  StateInit,
  StateNotConnected,
  StateWifiSetup,
  StateWifiConnecting,
  StateSetupWebServer,
  StateMqttConnect,
  StateConnected,
  StateOperating,
  StateIdle
};
ControlStateType ControlState = StateInit;

void loop()
{
  // Read the String until the JSON is fully sent;
  StaticJsonDocument<200> doc;
  readData(&doc);

  switch (ControlState)
  {
  case StateInit:

    // Serial.println("StateInit:");
    //    WiFi.mode(WIFI_STA);

    ControlState = StateNotConnected;
    break;

  case StateNotConnected:
    // Serial.println("StateNotConnected:");

    wifiConnectCounter = 0;
    ControlState = StateWifiSetup;
    break;

  case StateWifiSetup:
    Serial.println(F("StateWifiSetup:"));

    ControlState = StateWifiConnecting;
    break;

  case StateWifiConnecting:
    // Serial.println(F("StateMqttConnecting:"));
    ConnectWifi();

    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("");
      Serial.print("Connected to ");
      Serial.println(WiFi.SSID());
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      Serial.println();
      delay(1000);

      // WiFi.softAPdisconnect (true);
      Serial.println(WiFi.SSID());
      Serial.println(WiFi.localIP());
      /*
              if (!MDNS.begin(OTA_NAME))
              {
                Serial.println(F("mDNS init failed"));
              }
      */
      setupOTA();

      ControlState = StateSetupWebServer;
    }
    else
    {
      digitalWrite(LED_STATUS, !digitalRead(LED_STATUS));
      delay(400);
      Serial.print(F("."));
#if 1
      if (wifiConnectCounter++ > 30)
      {
        // giving up
        ESP.restart();
      }
#endif
    }

    break;

  case StateSetupWebServer:

    Serial.println(F("Starting Webserver ... "));
    ArduinoOTA.handle();

#if 1
    // Route for root / web page
    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                 { request->send(LittleFS, "/index.html"); });
    webServer.on("/snakeGame", HTTP_GET, [](AsyncWebServerRequest *request)
                 { request->send(LittleFS, "/snakeGame.html"); });
    webServer.on("/rawData", HTTP_GET, [](AsyncWebServerRequest *request)
                 { request->send(LittleFS, "/rawData.html"); });
    webServer.on("/formattedData", HTTP_GET, [](AsyncWebServerRequest *request)
                 { request->send(LittleFS, "/rawData.html"); });
#if 0
    webServer.on("/config", HTTP_GET, [](AsyncWebServerRequest *request)
                 { request->send(200, "application/x-www-form-urlencoded", "{\"test\":\"success\"}"); });

#endif
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");

    // Start server
    webServer.begin();
#endif
    webSocketService.begin();
    delay(200);

    ControlState = StateConnected;
    break;

  case StateConnected:
    Serial.println(F("StateConnected:"));

    ControlState = StateOperating;
    break;

  case StateOperating:
    // Serial.println("StateOperating:");

    if (WiFi.status() != WL_CONNECTED)
    {
      ControlState = StateWifiSetup;
      break; // exit (hopefully switch statement)
    }

    ArduinoOTA.handle();

    webSocketService.loop();

    break;

  case StateIdle:

    delay(10);
    break;

  default:
    Serial.println(F("Error: invalid ControlState"));
    ArduinoOTA.handle();
    delay(1);
  }
}

void readData(StaticJsonDocument<200> *doc)
{
  *doc = Serial2.readStringUntil('\n');
}