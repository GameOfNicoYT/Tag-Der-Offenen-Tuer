#ifndef _WEBSOCKETSERVICE_H_
#define _WEBSOCKETSERVICE_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WebSocketsServer.h>

enum class StatusCode
{
  NoSignal = 0,
  FrameError,
  DecryptionFailed,
  DataValid
};

class WebSocketService
{

private:
  static const uint16_t JSON_DATA_MAX = 512; // max size of websocket json string
  uint8_t lastJsonBuffer[JSON_DATA_MAX];     // save last sent websocket message
  bool lastJsonBufferValid = false;
  StatusCode lastStatus = StatusCode::NoSignal;      // status of meter values (NoSignal)
  WebSocketsServer webSocket = WebSocketsServer(81); // WebSocket

  void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);

public:
  WebSocketService();

  void begin(void);
  void loop(void);
};

#endif // _WEBSOCKETSERVICE_H_