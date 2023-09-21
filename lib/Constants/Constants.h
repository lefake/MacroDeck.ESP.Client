#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <Arduino.h>

#define DEBUG
#define DEBUG_STACK


// ========== STRIP MODULES ==========

#define NB_HARDWARE_STRIPS      2
#define NB_HARDWARE_MACRO       2

static const uint8_t sliderPins[NB_HARDWARE_STRIPS] = { 34, 35, };
static const uint8_t muteButtonPins[NB_HARDWARE_STRIPS] = { 4, 2, };   // { 4, 2, 15, 13, 12, 14, 27, 26 };
static const uint8_t muteLedPins[NB_HARDWARE_STRIPS] = { 23, 22, };     // { 23, 22, 21, 19, 18, 5, 17, 16 };

static const uint8_t macroButtonPins[NB_HARDWARE_MACRO] = { 27, 26, };   // { 4, 2, 15, 13, 12, 14, 27, 26 };

// ========== SPEEDS ==========

#define SLIDERS_POLLING_RATE    10
#define VM_POLLING_RATE         100
#define VM_PUSHING_RATE         100

// ========== HTTP / WIFI ==========

const String serverIP = "192.168.2.13";
const String serverPort = "5000";

// ========== VM ==========

#define VM_DB_MIN           (-60)
#define VM_DB_MAX           (12)

#endif