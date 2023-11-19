#include "StripModule.h"

uint8_t StripModule::hardwareId = 0;

uint16_t StripModule::init(const uint8_t* sPins, const uint8_t* bPins, const uint8_t* lPins, const uint8_t nb)
{
    uint16_t ret = OK;
    nbStrips = nb;

    for (uint8_t i = 0; i < nbStrips && GET_SEVERITY(ret) == SUCCESS; ++i)
        ret = strips[i].init(hardwareId++, sPins[i], bPins[i], lPins[i]);

    return ret;
}

uint16_t StripModule::update()
{
    uint16_t ret = OK;

    for (uint8_t i = 0; i < nbStrips && GET_SEVERITY(ret) == SUCCESS; ++i)
        ret = strips[i].update();

    return ret;
}

uint16_t StripModule::apply(double gains[], uint8_t mutes)
{
    uint16_t ret = OK;

    for (uint8_t i = 0; i < NB_HARDWARE_STRIPS && GET_SEVERITY(ret) == SUCCESS; ++i)
    {
        ret = strips[i].apply(gains[i], mutes & 0b1);
        mutes = mutes >> 1;
    }

    return ret;
}

uint16_t StripModule::getMutes(uint8_t *mutes)
{
    uint16_t ret = NO_MUTE_UPDATE;
    bool mute;
    *mutes = 0;

    for (uint8_t i = 0; i < nbStrips; ++i)
    {
        if (strips[i].getMuteState(&mute) == OK)
        {
            *mutes |= mute << i;
            ret = OK;
        }
    }

    return ret;
}

uint16_t StripModule::getGains(double gains[])
{
    uint16_t ret = NO_SLIDER_UPDATE;
    double gain;

    for (uint8_t i = 0; i < nbStrips; ++i)
    {
        gains[i] = GAIN_VALUE_NOT_UPDATED;
        if (strips[i].getGainState(&gain) == OK)
        {
            gains[i] = gain;
            ret = OK;
        }
    }

    return ret;
}
