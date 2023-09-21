#ifndef MACRO_MODULE_H_
#define MACRO_MODULE_H_

#include <Arduino.h>
#include "Constants.h"
#include "Button.h"

#ifndef NB_HARDWARE_MACRO
#define NB_HARDWARE_MACRO 10
#endif

class MacroModule
{
private:
    Button buttons[NB_HARDWARE_MACRO];
    bool buttonStates[NB_HARDWARE_MACRO];

    uint8_t nbButtons;
    static uint8_t hardwareId;

    void buttonPressCb(uint8_t id);

public:
    bool init(const uint8_t* bPins, const uint32_t nb);
    bool update();
    bool apply();
    bool getCurrentURI(String* uri);
};

#endif