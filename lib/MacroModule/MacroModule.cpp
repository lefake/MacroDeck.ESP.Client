#include "MacroModule.h"

uint8_t MacroModule::hardwareId = 0;

bool MacroModule::init(const uint8_t* sPins, const uint8_t iPin, const uint32_t nb)
{
    return init(sPins, iPin, 255, nb);
}

bool MacroModule::init(const uint8_t* sPins, const uint8_t iPin, const uint8_t ePin, const uint32_t nb)
{
    bool ret = true;
    nbButtons = nb;

    if (ePin == 255)
        ret &= mux.init(sPins, nb);
    else
        ret &= mux.init(sPins, ePin, nb);

    for (uint8_t i = 0; i < nbButtons; ++i)
        ret &= buttons[i].init(hardwareId++, iPin, INPUT_PULLDOWN, HIGH, [this](uint8_t id) { buttonPressCb(id); });

    return ret;
}

bool MacroModule::update()
{
    bool ret = true;
    mux.select(0);
    for (uint8_t i = 0; i < nbButtons; ++i)
    {
        ret &= buttons[i].update();
        mux.next();
    }

    return ret;
}

bool MacroModule::apply()
{
    return true;
}

bool MacroModule::getCurrentURI(String* uri)
{
    *uri = "/pushMacro?m=";

    uint32_t mute = 0;

    for (uint8_t i = 0; i < nbButtons; ++i)
    {
        if (buttonStates[i])
        {
            mute |= buttonStates[i] << i;
            buttonStates[i] = false;
        }
    }

    if (mute != 0)
        Serial.printf("Sending %i\n", mute);

    *uri += String(mute);

    return mute != 0;
}

void MacroModule::buttonPressCb(uint8_t id)
{
    buttonStates[id] = true;
    Serial.printf("Trigged %i\n", id);
}