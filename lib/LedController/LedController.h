// Led Controller
//
// Independent fonction wich control an LED
// There it will be possible to have nine modes
// OFF, ON, BLINK, FLASH 1,2,3 and the flashing inverse
//
// See below how to use it!!!
// 
// #define GPIO XX
//
// struct LedParam LedRed, *PLedRed=&LedRed;
//
// void setup()
// {
//      pinMode(GPIO, OUTPUT);
//      SetLedParam(PLedRed, FLASH_THREE, 150, 2000);   // Led will flashing three time, flashing as dead time will be 150ms and will repeat all 2s. 
// }
//
// void loop()
// {
//      GPIOController(PLedRed, GPIO);
//  }
//

#include <Arduino.h>

#define _ON 0
#define _OFF 1

enum OpMode {OFF, ON, BLINK, FLASH_ONE, FLASH_TWO, FLASH_THREE, FLASH_ONE_INV, FLASH_TWO_INV, FLASH_THREE_INV};

struct LedParam
{
    enum OpMode Mode;       // Operating mode

    char MaxOperationStep;  // Number of operation step 
    bool Init;
    bool Etat;              // Output Stata
    int tOn;                // On Time in ms
    int tRep;               // Repetition Time in ms

    long TimeLapse;
    long PreviousTime;
};

void SetLedParam (struct LedParam *_Led, enum OpMode _OpMode, int _tOn, int _tRep);
void SetLedMode (struct LedParam *_Led, enum OpMode _OpMode);
void GPIOController(struct LedParam *_LedParam, const byte _Led);