#include "Mux.h"

uint16_t Mux::init(const uint8_t* sPins, const uint8_t ePin, const uint8_t nbV)
{
    enablePin = ePin;
    nbValidIn = nbV;
    currentId = 0;

    if (enablePin != 255)
        pinMode(enablePin, OUTPUT);
    disable();

    for (uint8_t i = 0; i < MUX_EXP; ++i)
    {
        selectPins[i] = sPins[i];
        pinMode(selectPins[i], OUTPUT);
        digitalWrite(selectPins[i], LOW);
    }

    return OK;
}

uint16_t Mux::init(const uint8_t* sPins, const uint8_t nbV)
{
    return init(sPins, 255, nbV);
}

uint16_t Mux::select(uint8_t id)
{
    currentId = id;

    disable();
    
    for (uint8_t i = 0; i < MUX_EXP; ++i)
        digitalWrite(selectPins[i], (currentId >> i) & 0b1);

    vTaskDelay(SWITCH_TIME / portTICK_PERIOD_MS);

    return OK;
}

uint16_t Mux::next()
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
