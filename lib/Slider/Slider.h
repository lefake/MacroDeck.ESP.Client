#ifndef SLIDER_H_
#define SLIDER_H_

#include <Arduino.h>
#include "Constants.h"
#include "ErrorCode.h"

class Slider
{
private:
    uint8_t pin;

    uint8_t rollingIndex = 0;
    double rollingMean[ROLLING_LENGTH] = { 0 };
    double rollingSum = 0;

    double currentdB;
    double lastdB;
    
    uint16_t readValue();
    double updateRolling(double value);
    double dBMapping(double in);
    double pourcentMapping(double in);

public:
    uint16_t init(const uint8_t pin);
    uint16_t update();
    uint16_t getCurrent(double *current);
};

#endif