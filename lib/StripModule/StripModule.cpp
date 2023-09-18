#include "StripModule.h"

#include "utils.h"

StripModule::StripModule() : buttonChanged(false)
{
}

StripModule::~StripModule()
{
}

void StripModule::init(const uint8_t sId, const uint8_t sliderPin, const uint8_t buttonPin, const uint8_t muteLedPin)
{
    stripId = sId;
    slider.init(sliderPin);
    button.init(buttonPin, [this]() { muteCallback(); });
    ledPin = muteLedPin;

    pinMode(ledPin, OUTPUT);
}

void StripModule::update()
{
    button.update();
    slider.update();
}

bool StripModule::getPushURI(String* uri)
{
    double gain = 0;
    bool dataChanged = slider.getCurrent(&gain);
    *uri = "/push_strip?s=" + String(stripId);

    if(dataChanged)
        *uri += "&g=" + String(gain);

    if (buttonChanged)
    {
        *uri += "&m";
        buttonChanged = false;
        dataChanged = true;
    }

    return dataChanged;
}

void StripModule::apply(String body)
{
    digitalWrite(ledPin, char2Bool(body.charAt(0)));
}

String StripModule::getPullURI()
{
    return "/pull_strip?s=" + String(stripId);
}

void StripModule::muteCallback()
{
    buttonChanged = true;
}
