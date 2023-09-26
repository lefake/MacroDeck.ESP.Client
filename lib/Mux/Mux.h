#ifndef MUX_H_
#define MUX_H_

#include <Arduino.h>
#include "Constants.h"
#include "ErrorCode.h"

#ifndef SWITCH_TIME
#define SWITCH_TIME     1
#endif

class Mux
{
    private:
        uint8_t selectPins[MUX_EXP];
        uint8_t enablePin;
        uint8_t currentId;
        uint8_t nbValidIn;
    public:
        uint16_t init(const uint8_t* sPins, const uint8_t ePin, const uint8_t nbV);
        uint16_t init(const uint8_t* sPins, const uint8_t nbV);

        void enable();
        void disable();
        uint16_t select(uint8_t id);
        uint16_t next();
};

#endif