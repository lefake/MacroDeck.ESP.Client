#include "MacroModule.h"

uint8_t MacroModule::hardwareId = 0;

bool MacroModule::init(const uint8_t bPins[NB_HARDWARE_MACRO], const uint32_t nb)
{
    bool ret = true;
    nbButtons = nb;

    for (uint8_t i = 0; i < nbButtons; ++i)
        ret &= buttons[i].init(hardwareId++, bPins[i], [this](uint8_t id) { buttonPressCb(id); });

    return ret;
}

bool MacroModule::update()
{
    for (uint8_t i = 0; i < nbButtons; ++i)
        buttons[i].update();

    return true;
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

    *uri += String(mute);

    return mute != 0;
}

void MacroModule::buttonPressCb(uint8_t id)
{
    buttonStates[id] = true;
}