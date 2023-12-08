#ifndef MACRO_MODULE_H_
#define MACRO_MODULE_H_

#include <Arduino.h>
#include "Constants.h"
#include "Mux.h"
#include "Button.h"

class MacroModule
{
private:
    Mux mux;
    Button buttons[NB_HARDWARE_MACRO];
    bool buttonStates[NB_HARDWARE_MACRO];

    uint8_t inputPin;
    uint8_t nbButtons;
    static uint8_t hardwareId;
    uint8_t currentId; 

    void buttonPressCb(uint8_t id);
public:
    uint16_t init(const uint8_t* sPins, const uint8_t iPin, const uint32_t nb);
    uint16_t init(const uint8_t* sPins, const uint8_t iPin, const uint8_t ePin, const uint32_t nb);
    uint16_t update();
    uint16_t apply();
    uint16_t getMacros(uint8_t* macros);
};

#endif