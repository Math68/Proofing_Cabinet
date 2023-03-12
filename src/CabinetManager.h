#pragma once

#include "LedController.h"
#include "websocket.h"

#define DEBUG 1

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

#define GPIO 32

#define SW_OFF 0
#define SW_ON 1

#define THSW_OPEN 1
#define THSW_CLOSED 0

const int LedRed_IO = 15;
const int LedGreen_IO = 13;

const int ThermoSwitch_1 = 33;
const int ThermoSwitch_2 = 25;
const int TempSensor = 32; // it was 26 ADC2 but is in use by Wi-Fi, has to be at ADC1
const int Synchronisation = 27;
const int HeaterCtrl = 14;
const int RelayPhase = 2;
const int RelayNeutral = 19;
const int RelaySerie=17;

extern int TempValue;
//extern int TempMax;   // equivalent to °C
//extern int TempMin;
extern int TresholdLow;
extern int TresholdHigh;


extern String RunModeState;

enum RunMode {COOLING=0,HEATING};
extern RunMode RUNMODE;

enum Power {PWR_OFF=0, PWR_LOW, PWR_MID, PWR_HIGH};
//Power ACTUALPWR=PWR_MID, POWER=PWR_OFF;

extern struct LedParam LedRed, LedGreen;

extern long CabinetTimeLapse;
extern long PreviousMillis;

extern int ESP32_ADC_Offset;
extern int Counter;

void GetCabinetTemp();
void SetPowerAndLed(CabinetWebsocket *websocket);
void SetHeater(enum Power _Power);
void SaveTresholdLowToEEPROM(int THL);
void SaveTresholdHighToEEPROM(int THH);
void SetTreshold();
void InitMemory();