#ifndef STRIP_H_
#define STRIP_H_

#include <Arduino.h>
#include "ErrorCode.h"
#include "Slider.h"
#include "Button.h"
#include "Led.h"

class Strip
{
private:
    Slider slider;
    Button button;
    Led muteLed;
    uint8_t hardwareId;
    bool buttonTrigged;

    void buttonPressCb();
public:
    uint16_t init(uint8_t hId, const uint8_t sPin, const uint8_t bPin, const uint8_t lPin);
    uint16_t update();
    uint16_t apply(double gain, bool mute);
    uint16_t getState(double *gain, bool *mute);
    uint8_t getHardwareId();
};

#endif