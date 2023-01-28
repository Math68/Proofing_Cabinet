
#include "CabinetManager.h"
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
//#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "Freebox-372EBF";
const char* password = "mfrfzq7db9q43xzrqmv49b";

//const char* ssid = "EMCP-Guest";
//const char* password = "92*Shir#kh@n!07";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

String RunModeState="Cooling";

void notifyClients(String Data)
{
  ws.textAll(Data);
}

void handelWebSocketMessage(void *arg, uint8_t *data, size_t len){
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if(info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT){

    data[len] = 0;
    
    if(strcmp((char*)data, "RunModeState")==0){
      notifyClients(RunModeState);
    } else if (strcmp((char*)data, "powerOff")==0){
      ///TODO
    }
  }
}

void sendInitialData(AsyncWebSocketClient *client){
  if (RUNMODE == COOLING) {
    client->text("Cooling");
  } else if (RUNMODE == HEATING) {
    client->text("Heating");
  }

  String CabinetTemp = "temp:" + String(TempValue);
  client->text(CabinetTemp);
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
  switch(type){
    case WS_EVT_CONNECT:
      sendInitialData(client);
    break;
    case WS_EVT_DISCONNECT:
    break;
    case WS_EVT_DATA:
      handelWebSocketMessage(arg, data, len);
    break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
    break;
  }
}

void initWebSocket(){
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup()
{
  // ********** Serial **********
  Serial.begin(115200);
  
  // ********** GPIO **********
  // Heater Relais
  pinMode(RelaySerie, OUTPUT);
  digitalWrite(RelaySerie, SW_OFF);
  delay(250);
  pinMode(RelayPhase, OUTPUT);
  pinMode(RelayNeutral, OUTPUT);
  digitalWrite(RelayPhase, SW_OFF);
  digitalWrite(RelayNeutral, SW_OFF);
  delay(250);

  pinMode(GPIO, OUTPUT);
  pinMode(LedRed_IO, OUTPUT);
  pinMode(LedGreen_IO, OUTPUT);

  pinMode(ThermoSwitch_1, INPUT);
  pinMode(ThermoSwitch_2, INPUT);
  pinMode(TempSensor, INPUT);
  pinMode(Synchronisation, INPUT);
  pinMode(HeaterCtrl, INPUT);
  
  // ********** Set ADC **********
  analogSetAttenuation(ADC_6db);
  analogReadResolution(12);
  
  // ********** Set Leds **********
  SetLedParam(PLedRed, OFF, 150, 2000);
  SetLedParam(PLedGreen, ON, 100, 1000);

  // ********** Init Variable **********
  SetHeater(PWR_OFF);
  CabinetTimeLapse = 15000;
  PreviousMillis = millis();

  // ********** Spiffs **********
  if(!SPIFFS.begin())
  {
    debugln("Erreur SPIFFS...");
    return;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while(file)
  {
    debug("File: ");
    debugln(file.name());
    file.close();
    file = root.openNextFile();
  }

  // ********** Wifi **********
  WiFi.begin(ssid, password);
  debug("Tentative de connexion...");

  while(WiFi.status() != WL_CONNECTED)
  {
    debug(".");
    delay(100);
  }

  debugln("\n");
  debugln("Connexion etablie...");
  debug("Adresse IP: ");
  debugln(WiFi.localIP());

  initWebSocket();

  // ********** Server **********
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/w3.css", "text/css");
  });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/script.js", "text/javascript");
  });

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/thermo.html", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/thermo.html", "text/html");
  });

  server.on("/power.html", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/power.html", "text/html");
  });

  server.serveStatic("/",SPIFFS, "/");
  server.begin();
  debugln("Serveur actif!");
}

void loop()
{
  ws.cleanupClients();
  // debugln("--------------------");
  // debugln("Passage dans Loop");
  GPIOController(PLedRed, LedRed_IO);
  GPIOController(PLedGreen, LedGreen_IO);

  if ((millis() - PreviousMillis) >= CabinetTimeLapse)
  {
    PreviousMillis = millis();
    Counter++;

    debugln("Passage dans boucle");
    debugln(Counter);

    GetCabinetTemp();
    SetPowerAndLed();

    String CabinetTemp = "temp:" + String(TempValue);
    notifyClients(CabinetTemp);
  }
}

void GetCabinetTemp()
{
  /*
    ADC 12 bits => 4096 Steps => 3300mV/4095 => 1 bit = 805.6uV

    @0°C PT1000=1000R
    @10°C PT1000=1039R
    @50°C PT1000=1194R

    Data = 3.875 x °C + 1000

    => °C = (TempValue-1000)/3.875
  */

  int ReadmV = analogReadMilliVolts(TempSensor);
  debugln("Tension PT1000");
  debugln(ReadmV);
  // Temparature calculation
  // x2 due to I=500uA

  TempValue = (((ReadmV - ESP32_ADC_Offset) * 2) - 1000) / 3.875;

  debugln("Temperature: ");
  debugln(TempValue);
}

void SetPowerAndLed()
{
  debugln("Run mode =");
  debugln(RUNMODE);
  //debugln("Thermo Switch NC 40°C =");
  //debugln(digitalRead(ThermoSwitch_1));
  //debugln("Thermo Switch NO 60°C =");
  //debugln(digitalRead(ThermoSwitch_2));
  
  if (RUNMODE == HEATING)
  {
    if (TempValue >= TempMax)
    {
      RUNMODE = COOLING;
      // Notify
      notifyClients("Cooling");
      SetLedMode(PLedRed, FLASH_THREE_INV);
      SetLedMode(PLedGreen, OFF);
      SetHeater(PWR_OFF);
    }       
  }
  else if (RUNMODE == COOLING)
  {
    if (TempValue <= TempMin)
    {
      RUNMODE = HEATING;
      // Notify
      notifyClients("Heating");
      SetLedMode(PLedRed, ON);
      SetLedMode(PLedGreen, OFF);
      SetHeater(PWR_HIGH);
    }
  }
}

void SetHeater(enum Power _Power)
{
  //if(POWER != ACTUALPWR)
  //{
//    ACTUALPWR = POWER;

    switch (_Power)
    {
    case PWR_OFF:
      //digitalWrite(RelaySerie, SW_OFF);
      //delay(150);
      digitalWrite(RelayPhase, SW_OFF);
      digitalWrite(RelayNeutral, SW_OFF);
      delay(150);
      break;

    case PWR_LOW:
      digitalWrite(RelayPhase, SW_OFF);
      digitalWrite(RelayNeutral, SW_OFF);
      delay(150);
      //digitalWrite(RelaySerie, SW_ON);
      //delay(150);
      break;

    case PWR_MID:
      //digitalWrite(RelaySerie, SW_OFF);
      //delay(150);
      digitalWrite(RelayPhase, SW_OFF);
      delay(150);
      digitalWrite(RelayNeutral, SW_ON);
      delay(100);
      break;

    case PWR_HIGH:
      //digitalWrite(RelaySerie, SW_OFF);
      //delay(150);
      digitalWrite(RelayPhase, SW_ON);
      delay(150);
      digitalWrite(RelayNeutral, SW_ON);
      delay(100);
      break;
    }
 // }
}