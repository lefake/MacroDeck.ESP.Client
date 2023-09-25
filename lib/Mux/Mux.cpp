#include "Mux.h"

bool Mux::init(const uint8_t* sPins, const uint8_t ePin, const uint8_t nbV)
{
    enablePin = ePin;
    nbValidIn = nbV;
    currentId = 0;

    pinMode(enablePin, OUTPUT);
    disable();

    for (uint8_t i = 0; i < MUX_EXP; ++i)
    {
        selectPins[i] = sPins[i];
        pinMode(selectPins[i], OUTPUT);
        digitalWrite(selectPins[i], LOW);
    }

    return true;
}

bool Mux::init(const uint8_t* sPins, const uint8_t nbV)
{
    enablePin = 255;
    nbValidIn = nbV;
    currentId = 0;

    for (uint8_t i = 0; i < MUX_EXP; ++i)
    {
        selectPins[i] = sPins[i];
        pinMode(selectPins[i], OUTPUT);
        digitalWrite(selectPins[i], LOW);
    }

    return true;
}

bool Mux::select(uint8_t id)
{
    currentId = id;
    for (uint8_t i = 0; i < MUX_EXP; ++i)
        digitalWrite(selectPins[i], (currentId >> i) & 0b1);

    vTaskDelay(SWITCH_TIME / portTICK_PERIOD_MS);

    return true;
}

bool Mux::next()
{
    return select((currentId + 1) % nbValidIn);
}

void Mux::enable()
{
    if (enablePin != 255)
        digitalWrite(enablePin, MUX_ENABLE);
}

void Mux::disable()
{
    if (enablePin != 255)
        digitalWrite(enablePin, MUX_DISABLE);
}
