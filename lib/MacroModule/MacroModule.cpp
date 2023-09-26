#include "MacroModule.h"

uint8_t MacroModule::hardwareId = 0;

uint16_t MacroModule::init(const uint8_t* sPins, const uint8_t iPin, const uint32_t nb)
{
    return init(sPins, iPin, 255, nb);
}

uint16_t MacroModule::init(const uint8_t* sPins, const uint8_t iPin, const uint8_t ePin, const uint32_t nb)
{
    uint16_t ret;
    nbButtons = nb;

    if (ePin == 255)
        ret = mux.init(sPins, nb);
    else
        ret = mux.init(sPins, ePin, nb);

    if (ret == OK)
    {
        for (uint8_t i = 0; i < nbButtons && ret == OK; ++i)
        {
            ret = buttons[i].init(hardwareId++, iPin, INPUT_PULLDOWN, HIGH, [this](uint8_t id) { buttonPressCb(id); });
        }
    }

    return ret;
}

uint16_t MacroModule::update()
{
    uint16_t ret = mux.select(0);

    for (uint8_t i = 0; i < nbButtons && ret == OK; ++i)
    {
        mux.enable();
        ret = buttons[i].update();
        if (ret == OK)
            ret = mux.next();
    }

    return ret;
}

uint16_t MacroModule::apply()
{
    return OK;
}

uint16_t MacroModule::getCurrentURI(String* uri)
{
    *uri = "/pushMacro?m=";

    uint32_t macro = 0;

    for (uint8_t i = 0; i < nbButtons; ++i)
    {
        if (buttonStates[i])
        {
            macro |= buttonStates[i] << i;
            buttonStates[i] = false;
        }
    }

    *uri += String(macro);

    return macro != 0 ? OK : NO_MACRO_UPDATE;
}

void MacroModule::buttonPressCb(uint8_t id)
{
    buttonStates[id] = true;
}