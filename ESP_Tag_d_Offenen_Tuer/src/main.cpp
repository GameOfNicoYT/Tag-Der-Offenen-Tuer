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

/*
Here you can define your Build mode:
true = DEV           false = release
*/
#define dev true

#define OTA_NAME "NULL"
#define LED_STATUS 0
bool messageComplete;
String incomingMessage;

WebSocketService webSocketService;
HardwareSerial transfare(1);

void sendStatus(String status)
{

  if (status == "s")
  {
    transfare.println("snake");
  }
  else
  {
    transfare.println("normal");
  }
}

void readData()
{
  while (transfare.available() && !messageComplete)
  {
    char inChar = (char)transfare.read();

    if (inChar == ';')
    {
      messageComplete = true;
    }
    if (inChar >= 33 && inChar <= 125)
    {
      incomingMessage += inChar;
    }
  }

  if (messageComplete)
  {
#if dev
    Serial.println(incomingMessage);
#endif
    webSocketService.sendData(incomingMessage);

    messageComplete = false;
    incomingMessage = "";
  }
}

AsyncWebServer webServer(80);

uint8_t wifiConnectCounter = 0;

bool ConnectWifi(void)
{
  int i = 0;
  bool isWifiValid = false;
#if dev
  Serial.println("starting scan");
#endif
  // scan for nearby networks:
  int numSsid = WiFi.scanNetworks();
#if dev
  Serial.print("scanning WIFI, found ");
  Serial.print(numSsid);
  Serial.println(" available access points:");
#endif

  if (numSsid == -1)
  {
#if dev
    Serial.println("Couldn't get a wifi connection");
#endif
    return false;
  }

  for (int i = 0; i < numSsid; i++)
  {
#if dev
    Serial.print(i + 1);
    Serial.print(". ");
    Serial.print(WiFi.SSID(i));
    Serial.print("  ");
    Serial.println(WiFi.RSSI(i));
#endif
  }

  // search for given credentials
  for (CREDENTIAL credential : credentials)
  {
    for (int j = 0; j < numSsid; ++j)
    {
      if (strcmp(WiFi.SSID(j).c_str(), credential.ssid) == 0)
      {
#if dev
        Serial.print("credentials found for: ");
        Serial.println(credential.ssid);
#endif
        currentWifi = credential;
        isWifiValid = true;
      }
    }
  }

  if (!isWifiValid)
  {
#if dev
    Serial.println("no matching credentials");
#endif
    return false;
  }

// try to connect
#if dev
  Serial.println(WiFi.macAddress());
#endif
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
#if dev
    Serial.println("");
    Serial.print("Connecting to WiFi ");
    Serial.println(currentWifi.ssid);
#endif
  }

  i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(LED_STATUS, LOW);
    delay(300);
#if dev
    Serial.print(".");
#endif
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
#if dev
    Serial.print(F("Start updating "));
#endif
    if (ArduinoOTA.getCommand() == U_FLASH) {
#if dev
      Serial.println(F("sketch"));
#endif } else { // U_FS
#if dev
      Serial.println(F("filesystem"));
#endif
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    LittleFS.end(); });

  ArduinoOTA.onEnd([]()
                   {
#if dev
                     Serial.println(F("\nEnd"));
#endif
                   });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        {
#if dev
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
#endif
    digitalWrite(LED_STATUS, !digitalRead(LED_STATUS)); });

  ArduinoOTA.onError([](ota_error_t error)
                     {
#if dev
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println(F("Auth Failed"));
    else if (error == OTA_BEGIN_ERROR) Serial.println(F("Begin Failed"));
    else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect Failed"));
    else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive Failed"));
    else if (error == OTA_END_ERROR) Serial.println(F("End Failed")); });
#endif

  ArduinoOTA.begin();
}

void setupFilesystem()
{
  if (LittleFS.begin())
  {
#if dev
    Serial.println("FileSystem started");
#endif
  }
  else
  {
#if dev
    Serial.println("formatting FileSystem ...");
#endif
    LittleFS.format();
    ESP.restart();
  }
}

void setup()
{
  // pinMode(LED_STATUS, OUTPUT);

  Serial.begin(9600);
  transfare.begin(115200, SERIAL_8N1, 16, 17);
#if dev
  Serial.println("\nStarting WebServer - (" OTA_NAME ")");
#endif
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

  switch (ControlState)
  {
  case StateInit:

    ControlState = StateNotConnected;
    break;

  case StateNotConnected:

    wifiConnectCounter = 0;
    ControlState = StateWifiSetup;
    break;

  case StateWifiSetup:
#if dev
    Serial.println(F("StateWifiSetup:"));
#endif

    ControlState = StateWifiConnecting;
    break;

  case StateWifiConnecting:
    ConnectWifi();

    if (WiFi.status() == WL_CONNECTED)
    {
#if dev
      Serial.println("");
      Serial.print("Connected to ");
      Serial.println(WiFi.SSID());
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      Serial.println();
#endif
      delay(1000);

// WiFi.softAPdisconnect (true);
#if dev
      Serial.println(WiFi.SSID());
      Serial.println(WiFi.localIP());
#endif
      String ip = "{\"IP\": \"" + (String)WiFi.localIP() + "\", \"networkName\": \"" + (String)WiFi.SSID() + "\"}";
      setupOTA();
      transfare.println(ip);

      ControlState = StateSetupWebServer;
    }
    else
    {
      digitalWrite(LED_STATUS, !digitalRead(LED_STATUS));
      delay(400);
#if dev
      Serial.print(F("."));
#endif
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
#if dev
    Serial.println(F("Starting Webserver ... "));
#endif
    ArduinoOTA.handle();

#if 1
    // Route for root / web page
    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                 { request->send(LittleFS, "/index.html"); });
    webServer.on("/snakeGame", HTTP_GET, [](AsyncWebServerRequest *request)
                 { request->send(LittleFS, "/snakeGame.html"); });
    webServer.on("/rawData", HTTP_GET, [](AsyncWebServerRequest *request)
                 { request->send(LittleFS, "/rawData.html"); });
    webServer.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
                 { request->send(LittleFS, "/img/favicon.ico"); });
    webServer.on("/img/htl.png", HTTP_GET, [](AsyncWebServerRequest *request)
                 { request->send(LittleFS, "/img/htl.png"); });

#endif
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");

    // Start server
    webServer.begin();
    webSocketService.begin();
    delay(100);

    ControlState = StateConnected;
    break;

  case StateConnected:
#if dev
    Serial.println(F("StateConnected:"));
#endif

    ControlState = StateOperating;
    break;

  case StateOperating:
#if dev
// Serial.println("StateOperating:");
#endif
    readData();
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
#if dev
    Serial.println(F("Error: invalid ControlState"));
#endif
    ArduinoOTA.handle();
    delay(1);
  }
}
