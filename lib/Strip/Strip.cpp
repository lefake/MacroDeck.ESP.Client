#include "Strip.h"

uint16_t Strip::init(uint8_t hId, const uint8_t sPin, const uint8_t bPin, const uint8_t lPin)
{
    uint16_t ret;

    hardwareId = hId;
    buttonTrigged = false;

    ret = button.init(hardwareId, bPin, [this](uint8_t id) { buttonPressCb(); });
    if (ret == OK)
    {
        ret = slider.init(sPin);
        if (ret == OK)
            ret = muteLed.init(lPin);
    }

    return ret;
}

uint16_t Strip::update()
{
    uint16_t ret = button.update();

    if (ret == OK)
        ret = slider.update();
    
    return ret;
}

uint16_t Strip::apply(double gain, bool mute)
{
    __unused(gain); // Used if motorized sliders
    return muteLed.apply(mute);
}

uint16_t Strip::getState(double *gain, bool *mute)
{
    uint16_t gainUpdated = slider.getCurrent(gain);
    *mute = buttonTrigged;

    if (buttonTrigged)
        buttonTrigged = false;

    return gainUpdated;
}

uint8_t Strip::getHardwareId()
{
    return hardwareId;
}

void Strip::buttonPressCb()
{
    buttonTrigged = true;
}