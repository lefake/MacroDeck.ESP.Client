#ifndef CONSTANTS_H_
#define CONSTANTS_H_

// ========== DEBUG ==========
#define DEBUG
// #define DEBUG_STACK

// ========== BUTTONS ==========
#define DEBOUNCE_DELAY          25
#define BTN_ACTIVE              HIGH

// ========== SLIDERS ==========
#define ROLLING_LENGTH          25
#define CHANGE_THRES            (0.1)
#define DIR_MULTIPLIER          -1

#define ANALOG_LOW_THRESOLD     0
#define ANALOG_HIGH_THRESOLD    4095

// ========== LEDS ==========

// ========== MUX ==========
#define MUX_EXP                 3
#define MUX_DISABLE             HIGH
#define MUX_ENABLE              LOW
#define SWITCH_TIME             3

// ========== MODULES ==========
#define NB_HARDWARE_STRIPS      5
#define NB_HARDWARE_MACRO       6

#define GAIN_VALUE_NOT_UPDATED  (-100)

// ========== THREAD ==========
#define NB_TASKS                5
#define HARDWARE_POLLING_RATE   5
#define MQTT_POLLING_RATE       10
#define VM_PUSHING_RATE         100
#define MACRO_PUSHING_RATE      250
#define ERROR_HANDLING_RATE     500

// ========== WIFI ==========
#define WIFI_CONNECT_TIMEOUT     5000
#define MQTT_CONNECT_TIMEOUT     100

// ========== VOICEMEETER ==========
#define VM_DB_MIN               (12.0)
#define VM_DB_MAX               (-60.0)

// ========== MQTT ==========
#define NB_SUB_TOPICS           2

// ========== SLEEP ==========
#define uS_TO_S_FACTOR          1000000
#define S_TO_M_FACTOR           60
#define SLEEP_TIME              5
#define SLEEP_TIME_uS           (SLEEP_TIME * S_TO_M_FACTOR * uS_TO_S_FACTOR)
#define TIME_BEFORE_SLEEP_MS    60000

#endif