#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

class CabinetWebsocket
{
public:
    CabinetWebsocket(AsyncWebServer *_server, AsyncWebSocket *_ws);

    void initWebSocket();
    void notifyClients(String Data);

    void loop();

private:
    AsyncWebServer *server = nullptr;
    AsyncWebSocket *ws = nullptr;

    static void onWSEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
    void handleClientMessage(void *arg, uint8_t *data, size_t len);
    void sendInitialData(AsyncWebSocketClient *client);
};