#ifndef ERROR_CODE_H_
#define ERROR_CODE_H_

#include <stdint.h>

#define GEN_ERROR_CODE(severity, code)     ((uint16_t)((severity << 8) | code))
#define GET_SEVERITY(errorCode)             (((errorCode) >> 8) & 0xFF)
#define GET_CODE(errorCode)                 ((errorCode) & 0xFF)


#define SUCCESS                     0
#define WARNING                     1
#define ERROR                       2
#define FATAL                       3

// ==========  SUCCESS CODES ========== 
#define OK                          GEN_ERROR_CODE(SUCCESS, 0)
#define NO_BUTTON_UPDATE_READY      GEN_ERROR_CODE(SUCCESS, 1)
#define NO_LED_UPDATE               GEN_ERROR_CODE(SUCCESS, 2)
#define NO_MACRO_UPDATE             GEN_ERROR_CODE(SUCCESS, 3)

#define NO_SLIDER_UPDATE            GEN_ERROR_CODE(SUCCESS, 4)
#define NO_MUTE_UPDATE              GEN_ERROR_CODE(SUCCESS, 5)

#define STRIP_UPDATED               GEN_ERROR_CODE(SUCCESS, 6)
#define GAIN_UPDATED                GEN_ERROR_CODE(SUCCESS, 7)
#define MUTE_UPDATED                GEN_ERROR_CODE(SUCCESS, 8)
#define NO_STRIP_UPDATE             GEN_ERROR_CODE(SUCCESS, 9)

#define NO_STRIP_MODULE_UPDATE      GEN_ERROR_CODE(SUCCESS, 10)

// ==========  WARNING CODES ========== 

// ========== ERROR CODES ========== 
#define STRIP_UPDATE_ERROR          GEN_ERROR_CODE(ERROR, 1)

#define MQTT_TIMEOUT                GEN_ERROR_CODE(ERROR, 2)
#define MQTT_PUB_ERROR              GEN_ERROR_CODE(ERROR, 3)

#define HTTP_REFUSED                GEN_ERROR_CODE(ERROR, 4)
#define STRIP_MODULE_GET_ERROR      GEN_ERROR_CODE(ERROR, 5)


// ==========  FATAL CODES ========== 
#define WIFI_NOT_CONNECTED          GEN_ERROR_CODE(FATAL, 1)
#define OTA_ERROR                   GEN_ERROR_CODE(FATAL, 2)
#define INIT_FAILED                 GEN_ERROR_CODE(FATAL, 3)
#define MQTT_INIT_FAILED            GEN_ERROR_CODE(FATAL, 4)

#endif