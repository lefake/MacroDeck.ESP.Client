#include "Button.h"

bool Button::init(const uint8_t hId, const uint8_t pinId, ButtonCallback cb)
{
    hardwareId = hId;
    pin = pinId;
    callback = cb;

    currentState = BTN_ACTIVE;
    lastState = BTN_ACTIVE;
    lastDebounceTime = 0;
    lastUpdateTime = 0;

    pinMode(pin, INPUT_PULLUP);

    return true;
}

void Button::update()
{
    if (!isTimeToUpdate())
        return;

    int readState = digitalRead(pin);

    if (readState != lastState)
        lastDebounceTime = millis();

    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) 
    {
        if (readState != currentState)
        {
            currentState = readState;

            if(currentState == BTN_ACTIVE && callback != NULL)
                callback(hardwareId);
        }
    }

    lastState = readState;
}

bool Button::getState()
{
    return currentState;
}

bool Button::isTimeToUpdate() 
{
  return (millis() - lastDebounceTime) > (DEBOUNCE_DELAY / 8);
}