#include "MacroModule.h"

MacroModule::MacroModule()
{
}

MacroModule::~MacroModule()
{
}

void MacroModule::init(const uint8_t buttonPins[], const uint8_t ledPins[], const uint32_t nb)
{
    for (int i = 0; i < nb; ++i)
    {
        // TODO hId
        buttons[i].init(0, buttonPins[i], [this](uint8_t id) {});
        pinMode(ledPins[i], OUTPUT);
    }
}

void MacroModule::update()
{

}

void MacroModule::apply()
{

}

void MacroModule::getPushURI()
{

}

void MacroModule::getPullURI()
{

}
