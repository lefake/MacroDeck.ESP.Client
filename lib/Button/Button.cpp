#include "Button.h"

uint16_t Button::init(const uint8_t hId, const uint8_t pinId, ButtonCallback cb, bool initPin)
{
    hardwareId = hId;
    pin = pinId;
    activeState = BTN_ACTIVE;
    callback = cb;

    currentState = BTN_ACTIVE;
    lastState = BTN_ACTIVE;
    lastDebounceTime = 0;
    lastUpdateTime = 0;

    if (initPin)
        pinMode(pin, INPUT_PULLDOWN);

    return OK;
}

uint16_t Button::update()
{
    if (!isTimeToUpdate())
        return NO_BUTTON_UPDATE_READY;

    int readState = digitalRead(pin);

    if (readState != lastState)
        lastDebounceTime = millis();

    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) 
    {
        if (readState != currentState)
        {
            currentState = readState;

            if(currentState == activeState && callback != NULL)
                callback(hardwareId);
        }
    }

    lastState = readState;
    return OK;
}

bool Button::getState()
{
    return currentState;
}

bool Button::isTimeToUpdate() 
{
  return (millis() - lastDebounceTime) > (DEBOUNCE_DELAY / 8);
}