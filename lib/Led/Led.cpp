#include "Led.h"

bool Led::init(const uint8_t ledPin)
{
    pin = ledPin;
    currentState = LOW;
    
    pinMode(pin, OUTPUT);
    digitalWrite(pin, currentState);

    return true;
}

bool Led::apply(const bool state)
{
    if (currentState == state)
        return true;
    
    currentState = state;
    digitalWrite(pin, currentState);
    return true;
}
