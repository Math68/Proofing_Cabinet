#include "CabinetManager.h"

Preferences NVS_Lib;

int TempValue = 0;
int TresholdLow; //= 22;
int TresholdHigh; //= 27;

String RunModeState = "Cooling";
RunMode RUNMODE = COOLING;

struct LedParam LedRed, LedGreen;

long CabinetTimeLapse = 0;
long PreviousMillis = 0;

int ESP32_ADC_Offset = 2;
int Counter = -1;

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

void SetPowerAndLed(CabinetWebsocket *websocket)
{
  debugln("Run mode =");
  debugln(RUNMODE);
  //debugln("Thermo Switch NC 40°C =");
  //debugln(digitalRead(ThermoSwitch_1));
  //debugln("Thermo Switch NO 60°C =");
  //debugln(digitalRead(ThermoSwitch_2));
  
  if (RUNMODE == HEATING)
  {
    if (TempValue >= TresholdHigh)
    {
      RUNMODE = COOLING;
      // Notify
      websocket->notifyClients("Cooling");
      SetLedMode(&LedRed, FLASH_THREE_INV);
      SetLedMode(&LedGreen, OFF);
      SetHeater(PWR_OFF);
    }       
  }
  else if (RUNMODE == COOLING)
  {
    if (TempValue <= TresholdLow)
    {
      RUNMODE = HEATING;
      // Notify
      websocket->notifyClients("Heating");
      SetLedMode(&LedRed, ON);
      SetLedMode(&LedGreen, OFF);
      SetHeater(PWR_HIGH);
    }
  }
}

void SetHeater(enum Power _Power)
{
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
}

void SaveTresholdLow(int _THL)
{
  NVS_Lib.begin("STV",false);
  NVS_Lib.putInt("THL",_THL);
  NVS_Lib.end();
  TresholdLow=_THL;
}

void SaveTresholdHigh(int _THH)
{
  NVS_Lib.begin("STV",false);
  NVS_Lib.putInt("THH",_THH);
  NVS_Lib.end();
  TresholdHigh=_THH;
}