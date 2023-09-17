#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <Arduino.h>

#define DEBUG

// ========== BUTTONS / MACRO ==========

#define NB_BUTTONS              3
#define ACTIVE_STATE            LOW

#define BUTTONS_POLLING_RATE    10

static const uint8_t buttonPins[NB_BUTTONS] = { 17, 5, 18 };
static const bool buttonActiveState[NB_BUTTONS] = { ACTIVE_STATE, ACTIVE_STATE, ACTIVE_STATE };

// ========== SLIDERS / VOLUME ==========

#define NB_SLIDERS              1

#define SLIDERS_POLLING_RATE    10
#define SLIDERS_SENDING_RATE    1000

static const uint8_t sliderPins[NB_SLIDERS] = { 34 };

#endif