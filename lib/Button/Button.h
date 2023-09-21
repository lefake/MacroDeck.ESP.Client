#ifndef BUTTON_H_
#define BUTTON_H_

#include <Arduino.h>

#ifndef DEBOUNCE_DELAY
#define DEBOUNCE_DELAY      50
#endif

#ifndef BTN_ACTIVE
#define BTN_ACTIVE          LOW
#endif

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
    void update();
    bool getState();
};

#endif