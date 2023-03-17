#include "websocket.h"
#include "CabinetManager.h"

static CabinetWebsocket *wsinst = nullptr;

CabinetWebsocket::CabinetWebsocket(AsyncWebServer *_server, AsyncWebSocket *_ws):
    server(_server),
    ws(_ws)
{
    wsinst = this;
}

void CabinetWebsocket::notifyClients(String Data)
{
  ws->textAll(Data);
}

void CabinetWebsocket::onWSEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
  switch(type){
    case WS_EVT_CONNECT:
      wsinst->sendInitialData(client);
    break;
    case WS_EVT_DISCONNECT:
    break;
    case WS_EVT_DATA:
      wsinst->handleClientMessage(arg, data, len);
    break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
    break;
  }
}

void CabinetWebsocket::initWebSocket()
{
  ws->onEvent(onWSEvent);
  server->addHandler(ws);
}

void CabinetWebsocket::CabinetWebsocket::loop()
{
    ws->cleanupClients();
}

void CabinetWebsocket::handleClientMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if(info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {

    data[len] = 0;
    Serial.println(String((const char *)data));
    
    if(strcmp((char*)data, "RunModeState")==0)
    {
      wsinst->notifyClients(RunModeState);

    } else if (strcmp((char*)data, "powerOff")==0)
    {
      ///TODO
      
    } else if (strncmp((const char *)data, "SaveTresholdLow:", 16)==0)
    {
      char *strValue = (char*)(data + 16);
      int TempVal = atoi(strValue);

      if(TempVal<25)
        TempVal=25;
      else if(TempVal>31)
        TempVal=31;
      
      notifyClients("TresholdLow:" + String(TempVal, 10));
      SaveTresholdLow(TempVal);

    } else if (strncmp((const char *)data, "SaveTresholdHigh:", 17)==0)
    {
      char *strValue = (char*)(data + 17);
      int TempVal = atoi(strValue);
      if(TempVal<27)
        TempVal=27;
      else if(TempVal>35)
        TempVal=35;

      SaveTresholdHigh(TempVal);
      notifyClients("TresholdHigh:" + String(TempVal, 10));  
    } 
  }
}

void CabinetWebsocket::sendInitialData(AsyncWebSocketClient *client)
{
  if (RUNMODE == COOLING) {
    client->text("Cooling");
  } else if (RUNMODE == HEATING) {
    client->text("Heating");
  }

  String CabinetTemp = "temp:" + String(TempValue);
  client->text(CabinetTemp);
  notifyClients("TresholdLow:" + String(TresholdLow, 10));  // TresholLow en decimal, d'ou le 10 pour la base 10 
  notifyClients("TresholdHigh:" + String(TresholdHigh, 10));
}