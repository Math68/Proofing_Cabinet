#include "CabinetManager.h"
#include <Arduino.h>
#include "websocket.h"
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <Preferences.h>

Preferences ESP32_FlashMemory;

// Replace with your network credentials
const char* ssid = "Freebox-372EBF";
const char* password = "mfrfzq7db9q43xzrqmv49b";

//const char* ssid = "EMCP-Guest";
//const char* password = "92*Shir#kh@n!07";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
CabinetWebsocket websocket(&server, &ws);

void setup()
{
  // ********** Serial **********
  Serial.begin(115200);

  // Preferences Memory
  ESP32_FlashMemory.begin("TresholdValue",false);
  ESP32_FlashMemory.putInt("THL",255);
  ESP32_FlashMemory.putInt("THH",255);
  //TresholdLow = ESP32_FlashMemory.getInt("THL",25);
  //TresholdHigh = ESP32_FlashMemory.getInt("THH",28); 


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

  // ******* FLASH MEMORY ********
  
  // ********** Set ADC **********
  analogSetAttenuation(ADC_6db);
  analogReadResolution(12);
  
  // ********** Set Leds **********
  SetLedParam(&LedRed, OFF, 150, 2000);
  SetLedParam(&LedGreen, ON, 100, 1000);

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

  websocket.initWebSocket();

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
  websocket.loop();
  // debugln("--------------------");
  // debugln("Passage dans Loop");
  GPIOController(&LedRed, LedRed_IO);
  GPIOController(&LedGreen, LedGreen_IO);

  if ((millis() - PreviousMillis) >= CabinetTimeLapse)
  {
    PreviousMillis = millis();
    Counter++;

    debugln("Passage dans boucle");
    debugln(Counter);

    GetCabinetTemp();
    SetPowerAndLed(&websocket);

    String CabinetTemp = "temp:" + String(TempValue);
    websocket.notifyClients(CabinetTemp);
  }
}