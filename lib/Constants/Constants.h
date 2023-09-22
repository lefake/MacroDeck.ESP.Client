#ifndef CONSTANTS_H_
#define CONSTANTS_H_

// ========== DEBUG ==========
#define DEBUG
#define DEBUG_STACK

// ========== BUTTONS ==========
#define DEBOUNCE_DELAY          50
#define BTN_ACTIVE              LOW

// ========== SLIDERS ==========
#define ROLLING_LENGTH          50
#define CHANGE_THRES            (0.1)

#define ANALOG_LOW_THRESOLD     100
#define ANALOG_HIGH_THRESOLD    4095

// ========== LEDS ==========

// ========== MUX ==========
#define MUX_EXP                 3
#define MUX_NB_IN               8
#define MUX_DISABLE             HIGH
#define MUX_ENABLE              LOW

// ========== MODULES ==========

#define NB_HARDWARE_STRIPS      2
#define NB_HARDWARE_MACRO       2

// ========== THREAD ==========
#define SLIDERS_POLLING_RATE    10
#define VM_POLLING_RATE         100
#define VM_PUSHING_RATE         100

// ========== WIFI ==========
const String serverIP = "192.168.2.13";
const String serverPort = "5000";

// ========== VOICEMEETER ==========
#define VM_DB_MIN               (-60.0)
#define VM_DB_MAX               (12.0)

#endif