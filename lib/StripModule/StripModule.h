#ifndef STRIP_MODULE_H_
#define STRIP_MODULE_H_

#include <Arduino.h>
#include "Buttons.h"
#include "Sliders.h"

class StripModule
{
private:
    Sliders slider;
    Buttons button;
    uint8_t ledPin;

    uint8_t stripId;

    bool buttonChanged;

    void muteCallback();

public:
    StripModule();
    ~StripModule();

    void init(const uint8_t sId, const uint8_t sliderPin, const uint8_t buttonPin, const uint8_t muteLedPin);
    
    void update();
    bool getPushURI(String* uri);
    void apply(String body);
    String getPullURI();
};

#endif