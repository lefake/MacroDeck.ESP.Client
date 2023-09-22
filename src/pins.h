#ifndef PINS_H_
#define PINS_H_

#include <stdint.h>
#include "Constants.h"

// ========== Strip Module ==========
static const uint8_t sliderPins[NB_HARDWARE_STRIPS] = { 34, 35, };
static const uint8_t muteButtonPins[NB_HARDWARE_STRIPS] = { 4, 2, };   // { 4, 2, 15, 13, 12, 14, 27, 26 };
static const uint8_t muteLedPins[NB_HARDWARE_STRIPS] = { 23, 22, };     // { 23, 22, 21, 19, 18, 5, 17, 16 };

// ========== Macro Module ==========
static const uint8_t macroButtonPins[NB_HARDWARE_MACRO] = { 27, 26, };   // { 4, 2, 15, 13, 12, 14, 27, 26 };

// ========== Others ==========
static const uint8_t statusLedPin = 1;

#endif 