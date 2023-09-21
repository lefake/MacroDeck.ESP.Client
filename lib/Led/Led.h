#ifndef LED_H_
#define LED_H_

#include <Arduino.h>

class Led
{
    private:
        uint8_t pin;
        bool currentState;

    public:
        bool init(const uint8_t ledPin);
        bool apply(const bool state);
};

#endif 