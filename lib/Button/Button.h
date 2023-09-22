#ifndef BUTTON_H_
#define BUTTON_H_

#include <Arduino.h>
#include "Constants.h"

typedef std::function<void(uint8_t hId)> ButtonCallback;

class Button
{
private:
    uint8_t pin;
    uint8_t hardwareId;

    bool currentState;
    bool lastState;

    unsigned long lastDebounceTime;
    unsigned long lastUpdateTime;

    ButtonCallback callback = NULL;
    bool isTimeToUpdate();

public:
    bool init(const uint8_t hId, const uint8_t pinId, ButtonCallback cb);
    bool update();
    bool getState();
};

#endif