#include "Led.h"

uint16_t Led::init(const uint8_t ledPin)
{
    pin = ledPin;
    currentState = LOW;
    
    pinMode(pin, OUTPUT);
    digitalWrite(pin, currentState);

    return OK;
}

uint16_t Led::apply(const bool state)
{
    if (currentState == state)
        return NO_LED_UPDATE;
    
    currentState = state;
    digitalWrite(pin, currentState);
    return OK;
}

uint16_t Led::toggle()
{
    currentState = !currentState;
    digitalWrite(pin, currentState);
    return OK;
}
 