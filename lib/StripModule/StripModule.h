#ifndef STRIP_MODULE_H_
#define STRIP_MODULE_H_

#include <Arduino.h>
#include "Strip.h"

#ifndef NB_STRIPS
#define NB_STRIPS 5
#endif

class StripModule
{
private:
    Strip strips[NB_STRIPS];
    uint32_t nbStrips;

    static uint8_t hardwareId;

public:
    bool init(const uint8_t sPins[NB_STRIPS], const uint8_t bPins[NB_STRIPS], const uint8_t lPins[NB_STRIPS], const uint8_t nb);
    bool update();
    bool apply(String body);
    bool getCurrentURI(String *uri);
};

#endif