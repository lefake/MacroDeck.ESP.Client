#ifndef LED_H_
#define LED_H_

#include <Arduino.h>
#include "ErrorCode.h"

class Led
{
    private:
        uint8_t pin;
        bool currentState;

    public:
        uint16_t init(const uint8_t ledPin);
        uint16_t apply(const bool state);
        uint16_t toggle();
};

#endif 