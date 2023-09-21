#include "StripModule.h"
#include "utils.h"

uint8_t StripModule::hardwareId = 0;

bool StripModule::init(const uint8_t sPins[NB_STRIPS], const uint8_t bPins[NB_STRIPS], const uint8_t lPins[NB_STRIPS], const uint8_t nb)
{
    bool ret = true;
    nbStrips = nb;

    for (uint8_t i = 0; i < nbStrips; ++i)
        ret &= strips[i].init(hardwareId++, sPins[i], bPins[i], lPins[i]);

    return ret;
}

bool StripModule::update()
{
    bool ret = true;

    for (uint8_t i = 0; i < nbStrips; ++i)
        ret &= strips[i].update();

    return ret;
}

bool StripModule::apply(String body)
{
    bool ret = true;
    uint8_t mute = body.toInt();

    for (uint8_t i = 0; i < nbStrips; ++i)
    {
        ret &= strips[i].apply(0, mute & 0b1);
        mute = mute >> 1;
    }

    return ret;
}

bool StripModule::getCurrentURI(String* uri)
{
    bool ret = false;
    *uri = "/push?g=";

    double gains[nbStrips] = { 0 };
    uint8_t mute = 0;
    bool tempMute;

    for (uint8_t i = 0; i < nbStrips; ++i)
    {
        ret |= strips[i].getState(&gains[i], &tempMute);
        mute |= tempMute << i;
        *uri += String(gains[i]) + ',';
    }

    *uri += "&m=" + String(mute);

    return ret;
}