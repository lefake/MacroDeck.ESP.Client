#ifndef STRIP_H_
#define STRIP_H_

#include <Arduino.h>
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
    bool init(uint8_t hId, const uint8_t sPin, const uint8_t bPin, const uint8_t lPin);
    bool update();
    bool apply(double gain, bool mute);
    bool getState(double *gain, bool *mute);
    uint8_t getHardwareId();
};

#endif