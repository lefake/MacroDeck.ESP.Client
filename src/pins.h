#ifndef PINS_H_
#define PINS_H_

#include <stdint.h>
#include "Constants.h"

// ========== Strip Module ==========
static const uint8_t sliderPins[] = { 36, 39, 34, 35, 33 };
static const uint8_t muteButtonPins[] = { 23, 32, 25, 26, 27 };
static const uint8_t muteLedPins[] = { 17, 18, 19, 21, 22 };

// ========== Macro Module ==========
static const uint8_t muxSelectPins[MUX_EXP] = { 2, 4, 13 };
static const uint8_t muxInputPin = 16;

// ========== Others ==========
static const uint8_t statusLedPin = 12;

#endif 