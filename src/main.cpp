#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>

#include "Constants.h"
#include "config.h"
#include "pins.h"
#include "ErrorCode.h"

#include "ClientHttp.h"
#include "StripModule.h"
#include "MacroModule.h"

// Modules
static StripModule strips;
static MacroModule macros;
static Led statusLed;

// HTTP Client
static ClientHttp client(serverIP, serverPort);

static String pullBody;
static String pushURI;
static uint16_t fatalError = OK;

static int stackMin1 = 5000;
static int stackMin2 = 5000;
static int stackMin3 = 5000;
static int stackMin4 = 5000;

static TaskHandle_t pollHardwaregHandle;
static TaskHandle_t pollVMHandle;
static TaskHandle_t pushVMHandle;
static TaskHandle_t errorHandlingHandle;

static bool pollHardwareRun = true;
static bool pollVMRun = true;
static bool pushVMRun = true;

// ========== Functions ==========
static uint16_t connectWifi();
static void setupOTA();
static void errorNotify(uint16_t code);

// ===== Tasks =====
static void pollHardwareTask(void * parameter);
static void pollVMTask(void * parameter);
static void pushVMTask(void * parameter);
static void errorHandlingTask(void * parameter);

void setup()
{
    Serial.begin(115200);
    esp_sleep_enable_timer_wakeup(SLEEP_TIME_uS);

    uint16_t ret = statusLed.init(statusLedPin);
    ret |= strips.init(sliderPins, muteButtonPins, muteLedPins, NB_HARDWARE_STRIPS);
    ret |= macros.init(muxSelectPins, muxInputPin, NB_HARDWARE_MACRO);

    if (GET_SEVERITY(ret) == SUCCESS)
    {
        ret = connectWifi();
        if (ret == OK)
        {
            setupOTA();
            // Start threads
            xTaskCreate(pollHardwareTask, "Poll Hardware", 2000, NULL, 1, &pollHardwaregHandle);
            xTaskCreate(pollVMTask, "Poll VM", 2500, NULL, 1, &pollVMHandle);
            xTaskCreate(pushVMTask, "Push VM", 2500, NULL, 1, &pushVMHandle);
        }
        else
            fatalError = WIFI_NOT_CONNECTED;
    }
    else
        fatalError = INIT_FAILED;

    xTaskCreate(errorHandlingTask, "Error Handling", 2000, NULL, 2, &errorHandlingHandle);
}

void loop()
{
    ArduinoOTA.handle();
}

// ==================== Tasks ====================

void pollHardwareTask(void * parameter)
{
    uint16_t ret;
    while(pollHardwareRun)
    {
#ifdef DEBUG_STACK
        int reste = uxTaskGetStackHighWaterMark(NULL);

        if (reste < stackMin1)
        {
            stackMin1 = reste;
            Serial.printf("pollHardwareTask %i\n", stackMin1);
        }
#endif
        ret = strips.update();
        if (GET_SEVERITY(ret) == SUCCESS)
            ret = macros.update();

        if (GET_SEVERITY(ret) != SUCCESS)
            xTaskNotify(errorHandlingHandle, ret, eSetValueWithOverwrite);
        vTaskDelay(HARDWARE_POLLING_RATE / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void pollVMTask(void * parameter)
{
    uint16_t ret;
    while(pollVMRun)
    {
#ifdef DEBUG_STACK
        int reste = uxTaskGetStackHighWaterMark(NULL);

        if (reste < stackMin2)
        {
        stackMin2 = reste;
        Serial.printf("pollVMTask %i\n", stackMin2);
        }
#endif
        ret = client.httpGETRequest("/pull", &pullBody);

        if (GET_SEVERITY(ret) == SUCCESS)
            if (pullBody.length() > 0)
                ret = strips.apply(pullBody);

        if (GET_SEVERITY(ret) != SUCCESS)
            xTaskNotify(errorHandlingHandle, ret, eSetValueWithOverwrite);
        vTaskDelay(VM_POLLING_RATE / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void pushVMTask(void * parameter)
{
    uint16_t ret;
    while(pushVMRun)
    {
#ifdef DEBUG_STACK
        int reste = uxTaskGetStackHighWaterMark(NULL);

        if (reste < stackMin3)
        {
            stackMin3 = reste;
            Serial.printf("pushVMTask %i\n", stackMin3);
        }
#endif

        ret = strips.getCurrentURI(&pushURI);

        if (ret == OK)
            ret = client.httpPOSTRequest(pushURI, "");

        if (ret == OK)
            ret = macros.getCurrentURI(&pushURI);

        if (ret == OK)
            ret = client.httpPOSTRequest(pushURI, "");


        if (GET_SEVERITY(ret) != SUCCESS)
            xTaskNotify(errorHandlingHandle, ret, eSetValueWithOverwrite);
        vTaskDelay(VM_PUSHING_RATE / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void errorHandlingTask(void * parameter)
{
    uint32_t ulNotificationValue;
    uint16_t errorCode;
    uint16_t severity;
    uint16_t code;

    for(;;)
    {
#ifdef DEBUG_STACK
        int reste = uxTaskGetStackHighWaterMark(NULL);

        if (reste < stackMin4)
        {
            stackMin4 = reste;
            Serial.printf("errorHandlingTask %i\n", stackMin4);
        }
#endif
        if (fatalError != OK)
            while(1)
                errorNotify(GET_CODE(fatalError));

        ulNotificationValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (ulNotificationValue != 0)
        {
            errorCode = (uint16_t) ulNotificationValue;
            severity = GET_SEVERITY(errorCode);
            code = GET_CODE(errorCode);

            //Serial.printf("%i, %i\n", severity, code);

            switch (severity)
            {
            case SUCCESS:
            case WARNING:
                // Do nothing
                break;
            case ERROR:
                errorNotify(code);
                if (errorCode == HTTP_REFUSED)
                {
                    vTaskSuspend(pollHardwaregHandle);
                    vTaskSuspend(pollVMHandle);
                    vTaskSuspend(pushVMHandle);
                    esp_deep_sleep_start();
                }
                break;
            case FATAL:
            default:
                pollHardwareRun = false;
                pollVMRun = false;
                pushVMRun = false;
                fatalError = errorCode;
                break;
            }
        }
    }
}

// ==================== Private ====================
uint16_t connectWifi()
{
    IPAddress staticIP(ST_IP);
    IPAddress gateway(ST_GATEWAY);
    IPAddress subnet(ST_NETMASK);

    WiFi.config(staticIP, gateway, subnet);
    WiFi.setHostname(HOST_NAME);
    WiFi.begin(ssid, password);

    unsigned long start = millis();

    while(WiFi.status() != WL_CONNECTED && (millis() - start < WIFI_CONNECT_TIMEOUT));

    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.disconnect();
        return WIFI_NOT_CONNECTED;
    }

#ifdef DEBUG
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
#endif

    return OK;
}

void setupOTA()
{
    ArduinoOTA.setHostname(HOST_NAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.onError([](ota_error_t error) { fatalError = OTA_ERROR; });
    ArduinoOTA.begin();
}

void errorNotify(uint16_t code)
{
    int time = code * 100;
    statusLed.apply(HIGH);
    vTaskDelay(time / portTICK_PERIOD_MS);
    statusLed.apply(LOW);
    vTaskDelay(time / portTICK_PERIOD_MS);
}