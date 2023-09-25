#ifndef MUX_H_
#define MUX_H_

#include <Arduino.h>
#include "Constants.h"

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
        bool init(const uint8_t* sPins, const uint8_t ePin, const uint8_t nbV);
        bool init(const uint8_t* sPins, const uint8_t nbV);

        void enable();
        void disable();
        bool select(uint8_t id);
        bool next();
};

#endif