#include "WebSocketService.h"

WebSocketService::WebSocketService()
{
}

void WebSocketService::begin(void)
{
  webSocket.onEvent(std::bind(&WebSocketService::webSocketEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

  Serial.println(F("starting WebSockets"));
  // start webSocket server
  webSocket.enableHeartbeat(60000UL, 3000, 2);
  webSocket.begin();
}

void WebSocketService::loop(void)
{
  webSocket.loop();
}

void WebSocketService::webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.printf("[%u] Disconnected!\n\r", num);
    break;

  case WStype_CONNECTED:
  {
    IPAddress ip = webSocket.remoteIP(num);
    Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n\r", num, ip[0], ip[1], ip[2], ip[3], payload);

    StaticJsonDocument<256> jsonDoc;
    jsonDoc[F("status")] = F("conncected");
    char jstr[256];
    // serializeJson(jsonDoc, Serial);
    serializeJson(jsonDoc, jstr);
    webSocket.sendTXT(num, jstr);

    // -----------------------------------------------------------
    //                      SEND DATA

    // send message to client
    String recieveData;
    String sendData;

    StaticJsonDocument<200> doc;

    doc = Serial1.readStringUntil('\n');
    deserializeJson(doc, recieveData);
    serializeJson(doc, sendData);
    webSocket.sendTXT(num, sendData);

    break;
  }

  case WStype_TEXT:
  {
    Serial.printf("[%u] ws: %s\n\r", num, payload);
    // webSocket.broadcastTXT(payload);

    break;
  }
  case WStype_PING:
    // dont print payload --> exception
    // Serial.printf("[%u] PING \n", num);

    break;
  case WStype_PONG:
    // heartbeat answer from the ws-client
    // Serial.printf("[%u] PONG \n", num);

    break;
  default:
    Serial.print(F("unhandled websocket event: "));
    Serial.println(type);
  }
}

#if 0
void WebSocketService::broadcastStatus()
{
  char buf[JSON_DATA_MAX];
  DynamicJsonDocument jsonDoc(JSON_DATA_MAX);
  jsonDoc[F("type")] = F("status");
  jsonDoc[F("value")] = (uint8_t)lastStatus;
  serializeJson(jsonDoc, Serial);
  serializeJson(jsonDoc, buf);
  webSocket.broadcastTXT(buf);
}

void WebSocketService::broadcastStatus(StatusCode status)
{
  if (lastStatus == status) return;

  lastStatus = status;
  broadcastStatus();
}

void WebSocketService::sendVersion(uint8_t num)
{
  char buf[JSON_DATA_MAX];
  char version[30];
  snprintf(version, sizeof(version), "%s - %c, %d", VERSION, VARIANT, BAUDRATE);
  DynamicJsonDocument jsonDoc(JSON_DATA_MAX);
  jsonDoc[F("type")] = F("version");
  jsonDoc[F("value")] = version;
  Serial.println();
  serializeJson(jsonDoc, Serial);
  serializeJson(jsonDoc, buf);
  webSocket.sendTXT(num, buf);
}
#endif