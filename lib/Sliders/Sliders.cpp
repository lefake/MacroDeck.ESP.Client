#include "Sliders.h"

Sliders::Sliders() : lastdB(0), pin(255) { }

Sliders::~Sliders() { }

void Sliders::init(const uint8_t pinId)
{
  pin = pinId;
  lastdB = 0;

  for (int i = 0; i < ROLLING_LENGTH; ++i)
    update();
}

bool Sliders::getCurrent(double *current)
{
  bool ret = abs(lastdB - currentdB) > CHANGE_THRES;

  if (ret)
  {
    lastdB = currentdB;
    *current = currentdB;
  }

  return ret;
}

void Sliders::update()
{
    int value = readValue();
    double rawSmoothed = updateRolling(value);
    currentdB = dBMapping(rawSmoothed, ANALOG_LOW_THRESOLD, ANALOG_HIGH_THRESOLD, VM_DB_MIN, VM_DB_MAX);
}

double Sliders::updateRolling(double value)
{
  // Remove current index value from sum
  rollingSum -= rollingMean[rollingIndex];

  // Get current reading and update sum
  rollingMean[rollingIndex] = value;
  rollingSum += rollingMean[rollingIndex];

  // Increase index
  rollingIndex = (rollingIndex + 1 ) % ROLLING_LENGTH;

  return rollingSum / ROLLING_LENGTH;
}

uint16_t Sliders::readValue()
{
  uint16_t value = analogRead(pin);

  if (value < ANALOG_LOW_THRESOLD)
    value = ANALOG_LOW_THRESOLD;

  return value;
}