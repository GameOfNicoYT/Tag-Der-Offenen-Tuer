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

void WebSocketService::sendData(String data)
{
  webSocket.broadcastTXT(data);
  Serial.println("SENT DATA TO CLIENTS");
  delay(5);
}
