#include "openIot.h"
/**
 * Websocket connector
 */

/**
 * Callback for incoming messages. These will be passed to the elementmanager for further processing. 
 */
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    elementManager.handleWebEvent((char*)data);
  }
}

/*
* callback for websocket events.
*/
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  IPAddress ip;
  switch (type) {
    case WS_EVT_CONNECT:
      ip = client->remoteIP();
      iotCore.logDPrintln (F("WebSocket client %d connected from %u.%u.%u.%u"), client->id(), ip[0], ip[1], ip[2], ip[3]);
      elementManager.sendValues(client->id());
      break;
    case WS_EVT_DISCONNECT:
      iotCore.logDPrintln (F("WebSocket client %d disconnected."), client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}
