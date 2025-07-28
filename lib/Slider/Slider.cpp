#include "Slider.h"

uint16_t Slider::init(const uint8_t pinId)
{
  pin = pinId;
  lastMapped = 0;

  uint16_t ret = OK;

  for (int i = 0; i < ROLLING_LENGTH && ret == OK; ++i)
    ret = update();

  return ret;
}

uint16_t Slider::getCurrent(double *current)
{
  bool ret = abs(lastMapped - currentMapped) > CHANGE_THRES;
  
  if (ret)
  {
    lastMapped = currentMapped;
    *current = currentMapped;
  }
  else
    *current = lastMapped;

  return ret ? OK : NO_SLIDER_UPDATE;
}

uint16_t Slider::update()
{
    int value = readValue();
    double rawSmoothed = updateRolling(value);
    currentMapped = pourcentMapping(rawSmoothed);
    
    return OK;
}

double Slider::updateRolling(double value)
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

uint16_t Slider::readValue()
{
  uint16_t value = analogRead(pin);

  if (value < ANALOG_LOW_THRESOLD)
    value = ANALOG_LOW_THRESOLD;

  if (value > ANALOG_HIGH_THRESOLD)
    value = ANALOG_HIGH_THRESOLD;

  if (INVERTED_DIRECTION)
    value = ANALOG_HIGH_THRESOLD - value;

  return value;
}

double Slider::dBMapping(double in)
{
    double normalized = (in - ANALOG_LOW_THRESOLD) / (ANALOG_HIGH_THRESOLD - ANALOG_LOW_THRESOLD);
    double mappedValue = log10(1 + 9 * normalized);
    double dB = mappedValue * (VM_DB_MAX - VM_DB_MIN) + VM_DB_MIN;

    return round(dB * 100.0) / 100.0;
}

double Slider::pourcentMapping(double in)
{
    return map(in, ANALOG_LOW_THRESOLD, ANALOG_HIGH_THRESOLD, 0., 100.);
}