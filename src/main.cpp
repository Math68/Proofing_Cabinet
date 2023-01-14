
#include "CabinetManager.h"
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

void setup()
{
  Serial.begin(115200);
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

  analogSetAttenuation(ADC_6db);
  analogReadResolution(12);

  SetLedParam(PLedRed, OFF, 150, 2000);
  SetLedParam(PLedGreen, ON, 100, 1000);
  SetHeater(PWR_OFF);
  CabinetTimeLapse = 15000;
  PreviousMillis = millis();
}

void loop()
{
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

    GetCabinentTemp();
    SetPowerAndLed();
  }
}

void GetCabinentTemp()
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
  debugln("Thermo Switch NC 40°C =");
  debugln(digitalRead(ThermoSwitch_1));
  debugln("Thermo Switch NO 60°C =");
  debugln(digitalRead(ThermoSwitch_2));
  
  if (RUNMODE == HEATING)
  {
    if (TempValue >= TempMax)
    {
      RUNMODE = COOLING;
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