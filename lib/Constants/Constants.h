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
#define CHANGE_THRES            (0.5)

#define ANALOG_LOW_THRESOLD     100
#define ANALOG_HIGH_THRESOLD    4095

// ========== LEDS ==========

// ========== MUX ==========
#define MUX_EXP                 3
#define MUX_DISABLE             HIGH
#define MUX_ENABLE              LOW
#define SWITCH_TIME             1

// ========== MODULES ==========

#define NB_HARDWARE_STRIPS      5
#define NB_HARDWARE_MACRO       8

// ========== THREAD ==========
#define HARDWARE_POLLING_RATE   10
#define VM_POLLING_RATE         200
#define VM_PUSHING_RATE         100
#define ERROR_HANDLING_RATE     500

// ========== WIFI ==========
#define WIFI_CONNECT_TIMEOUT     2000
#define HTTP_REQUEST_TIMEOUT     (100)

// ========== VOICEMEETER ==========
#define VM_DB_MIN               (-60.0)
#define VM_DB_MAX               (12.0)

// ========== SLEEP ==========
#define uS_TO_S_FACTOR          1000000
#define S_TO_M_FACTOR           60
#define SLEEP_TIME              5
#define SLEEP_TIME_uS           (SLEEP_TIME * S_TO_M_FACTOR * uS_TO_S_FACTOR)

#endif