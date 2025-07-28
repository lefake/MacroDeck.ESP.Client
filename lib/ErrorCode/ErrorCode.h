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

// ==========  WARNING CODES ========== 

// ========== ERROR CODES ========== 
#define MQTT_TIMEOUT                GEN_ERROR_CODE(ERROR, 1)
#define MQTT_PUB_ERROR              GEN_ERROR_CODE(ERROR, 2)
#define MQTT_BAD_IN_MSG             GEN_ERROR_CODE(ERROR, 3)

// ==========  FATAL CODES ========== 
#define WIFI_NOT_CONNECTED          GEN_ERROR_CODE(FATAL, 3)
#define OTA_ERROR                   GEN_ERROR_CODE(FATAL, 4)
#define INIT_FAILED                 GEN_ERROR_CODE(FATAL, 5)
#define MQTT_INIT_FAILED            GEN_ERROR_CODE(FATAL, 6)
#define INVALID_MQTT_TOPIC          GEN_ERROR_CODE(FATAL, 7)
#define INVALID_STRIP_ID            GEN_ERROR_CODE(FATAL, 8)

#endif