#include "Buttons.h"

Buttons::Buttons() { }

Buttons::~Buttons() { }

void Buttons::init(const uint8_t pinId, const bool active, ButtonCallback cb)
{
    pin = pinId;
    activeState = active;
    callback = cb;

    currentState = activeState;
    lastState = activeState;
    lastDebounceTime = 0;
    lastUpdateTime = 0;

    pinMode(pin, INPUT_PULLUP);
}

void Buttons::update()
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

            if(currentState == activeState && callback != NULL)
                callback(pin);
        }
    }

    lastState = readState;
}

bool Buttons::getState()
{
    return currentState;
}

bool Buttons::isTimeToUpdate() 
{
  return (millis() - lastDebounceTime) > (DEBOUNCE_DELAY / 8);
}