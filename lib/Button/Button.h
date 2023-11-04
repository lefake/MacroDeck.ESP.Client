#ifndef BUTTON_H_
#define BUTTON_H_

#include <Arduino.h>
#include "Constants.h"
#include "ErrorCode.h"

typedef std::function<void(uint8_t hId)> ButtonCallback;

class Button
{
private:
    uint8_t pin;
    uint8_t hardwareId;
    bool activeState;

    bool currentState;
    bool lastState;

    unsigned long lastDebounceTime;
    unsigned long lastUpdateTime;

    ButtonCallback callback = NULL;
    bool isTimeToUpdate();

public:
    uint16_t init(const uint8_t hId, const uint8_t pinId, ButtonCallback cb);
    uint16_t update();
    bool getState();
};

#endif