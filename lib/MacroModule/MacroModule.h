#ifndef MACRO_MODULE_H_
#define MACRO_MODULE_H_

#include <Arduino.h>
#include "Button.h"

#ifndef NB_MACRO_BTN
#define NB_MACRO_BTN 10
#endif

class MacroModule
{
private:
    Button buttons[NB_MACRO_BTN];
    uint8_t ledPins[NB_MACRO_BTN];

public:
    MacroModule();
    ~MacroModule();

    void init(const uint8_t buttonPins[], const uint8_t ledPins[], const uint32_t nb);
    void update();
    void apply();
    void getPushURI();
    void getPullURI();
};

#endif