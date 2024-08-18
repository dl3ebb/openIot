#include "openIot.h"
#ifndef WebSocketHandler_h
#define WebSocketHandler_h

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);

#endif
