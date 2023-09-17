#include "Sliders.h"

Sliders::Sliders() { }

Sliders::~Sliders() { }

void Sliders::init(const uint8_t pinId)
{
  pin = pinId;
  lastdB = 0;

  for (int i = 0; i < ROLLING_LENGTH; ++i)
  {
    update();
  }
}

void Sliders::update()
{
    int value = readValue();
    double rawdB = todB(value);
    currentdB = updateRolling(rawdB);
}

bool Sliders::getCurrent(double *current)
{
  bool ret = abs(lastdB - currentdB) > CHANGE_THRES;

  lastdB = currentdB;
  *current = round2Digit(currentdB);

  return ret;
}

uint16_t Sliders::readValue()
{
    uint16_t value = analogRead(pin);
    if (value < 100)
    {
        value = 100;
    }

    return value;
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

double Sliders::todB(double value)
{
  double dB = 10 * log10(value);
  if (dB < MIN_DB)
    dB = MIN_DB;
  else if (dB > MAX_DB)
    dB = MAX_DB;

  return dB;
}

double Sliders::round2Digit(double value)
{
  int val = value * 100;
  return (double) val / 100.;
}