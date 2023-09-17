#ifndef BUTTONS_H_
#define BUTTONS_H_

#include <Arduino.h>

#ifndef DEBOUNCE_DELAY
#define DEBOUNCE_DELAY      50
#endif

typedef std::function<void(const int)> ButtonCallback;

class Buttons
{
private:
    uint8_t pin;
    bool activeState;

    bool currentState;
    bool lastState;

    unsigned long lastDebounceTime;
    unsigned long lastUpdateTime;

    ButtonCallback callback = NULL;
    bool isTimeToUpdate();

public:
    Buttons();
    ~Buttons();

    void init(const uint8_t pinId, const bool active, ButtonCallback cb);
    void update();
    bool getState();
};

#endif