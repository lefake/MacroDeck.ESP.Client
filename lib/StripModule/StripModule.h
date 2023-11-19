#ifndef STRIP_MODULE_H_
#define STRIP_MODULE_H_

#include <Arduino.h>
#include "Constants.h"
#include "Strip.h"

class StripModule
{
private:
    Strip strips[NB_HARDWARE_STRIPS];
    uint32_t nbStrips;

    static uint8_t hardwareId;

public:
    uint16_t init(const uint8_t* sPins, const uint8_t* bPins, const uint8_t* lPins, const uint8_t nb);
    uint16_t update();
    uint16_t apply(double gains[], uint8_t mutes);
    uint16_t getMutes(uint8_t *mutes);
    uint16_t getGains(double models[]);
};

#endif