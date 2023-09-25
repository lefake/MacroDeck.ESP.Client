#include "Strip.h"

bool Strip::init(uint8_t hId, const uint8_t sPin, const uint8_t bPin, const uint8_t lPin)
{
    bool ret = true;

    hardwareId = hId;
    buttonTrigged = false;

    ret &= button.init(hardwareId, bPin, [this](uint8_t id) { buttonPressCb(); });
    ret &= slider.init(sPin);
    ret &= muteLed.init(lPin);

    return ret;
}

bool Strip::update()
{
    return button.update() && slider.update();
}

bool Strip::apply(double gain, bool mute)
{
    __unused(gain); // Used if motorized sliders
    return muteLed.apply(mute);
}

bool Strip::getState(double *gain, bool *mute)
{
    bool gainUpdated = slider.getCurrent(gain);
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