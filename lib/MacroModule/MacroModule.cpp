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
            ret = buttons[i].init(hardwareId++, iPin, [this](uint8_t id) { buttonPressCb(id); });
        }
    }

    return ret;
}

uint16_t MacroModule::update()
{
    uint16_t ret = OK;

    for (uint8_t i = 0; i < nbButtons && ret == OK; ++i)
    {
        ret = mux.select(i);

        if (ret == OK)
        {
            mux.enable();
            ret = buttons[i].update();
        }
    }

    return ret;
}

uint16_t MacroModule::apply()
{
    return OK;
}

uint16_t MacroModule::getMacros(uint8_t* macros)
{
    uint16_t ret = NO_MACRO_UPDATE;
    *macros = 0;

     for (uint8_t i = 0; i < nbButtons; ++i)
    {
        if (buttonStates[i])
        {
            *macros |= buttonStates[i] << i;
            buttonStates[i] = false;
            ret = OK;
        }
    }

    return ret;
}

void MacroModule::buttonPressCb(uint8_t id)
{
    buttonStates[id] = true;
}