#include "StripModule.h"

uint8_t StripModule::hardwareId = 0;

bool StripModule::init(const uint8_t* sPins, const uint8_t* bPins, const uint8_t* lPins, const uint8_t nb)
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
    *uri = "/push?";

    double gains[nbStrips] = { 0 };
    uint8_t mute = 0;
    bool tempMute;

    for (uint8_t i = 0; i < nbStrips; ++i)
    {
        if (strips[i].getState(&gains[i], &tempMute))
        {
            if (!ret)
                *uri += "g=";
                
            *uri += String(strips[i].getHardwareId()) + ':' + (gains[i]) + ',';
            ret = true;
        }

        mute |= tempMute << i;
    }

    if (mute != 0)
    {
        if (ret)
            *uri += "&";

        *uri += "m=" + String(mute);
        ret = true;
    }

    return ret;
}