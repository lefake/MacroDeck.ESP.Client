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

uint16_t StripModule::apply(String body)
{
    uint16_t ret = OK;
    uint8_t mute = body.toInt();

    for (uint8_t i = 0; i < nbStrips && GET_SEVERITY(ret) == SUCCESS; ++i)
    {
        ret = strips[i].apply(0, mute & 0b1);
        mute = mute >> 1;
    }

    return ret;
}

uint16_t StripModule::getCurrentURI(String* uri)
{
    uint16_t ret = NO_STRIP_MODULE_UPDATE;
    *uri = "/push?";

    double gains[nbStrips] = { 0 };
    uint8_t mute = 0;
    bool tempMute;

    for (uint8_t i = 0; i < nbStrips; ++i)
    {
        if (strips[i].getState(&gains[i], &tempMute) == OK)
        {
            if (ret == NO_STRIP_MODULE_UPDATE)
                *uri += "g=";
                
            *uri += String(strips[i].getHardwareId()) + ':' + (gains[i]) + ',';
            ret = OK;
        }

        mute |= tempMute << i;
    }

    if (GET_SEVERITY(ret) == SUCCESS)
    {
        if (mute != 0)
        {
            if (ret == OK)
                *uri += "&";

            *uri += "m=" + String(mute);
            ret = OK;
        }
    }

    return ret;
}