#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>

#include "Constants.h"
#include "config.h"
#include "pins.h"
#include "errorCode.h"

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
static uint16_t errorId;

static int stackMin1 = 5000;
static int stackMin2 = 5000;
static int stackMin3 = 5000;
static int stackMin4 = 5000;

static TaskHandle_t pollHardwareHandle;
static TaskHandle_t pollVMHandle;
static TaskHandle_t pushVMHandle;

// ========== Functions ==========
static uint16_t connectWifi();
static void setupOTA();
static void errorHandler(uint16_t code);

// ===== Tasks =====
static void pollVMTask(void * parameter);
static void pushVMTask(void * parameter);
static void pollHardwareTask(void * parameter);
static void errorHandlingTask(void * parameter);

void setup()
{
    Serial.begin(115200);

    // Setup WiFi and OTA
    uint16_t ret = connectWifi();

    setupOTA();

    // Init modules
    strips.init(sliderPins, muteButtonPins, muteLedPins, NB_HARDWARE_STRIPS);
    macros.init(muxSelectPins, muxInputPin, NB_HARDWARE_MACRO);
    statusLed.init(statusLedPin);

    // Start threads
    xTaskCreate(pollHardwareTask, "Poll Hardware", 5000, NULL, 1, &pollHardwareHandle);
    xTaskCreate(pollVMTask, "Poll VM", 5000, NULL, 1, &pollVMHandle);
    xTaskCreate(pushVMTask, "Push VM", 5000, NULL, 1, &pushVMHandle);
    xTaskCreate(errorHandlingTask, "Error Handling", 5000, NULL, 1, NULL);
}

void loop()
{
    // OTA
    ArduinoOTA.handle();
}

void pollVMTask(void * parameter)
{
    for(;;)
    {
#ifdef DEBUG_STACK
        int reste = uxTaskGetStackHighWaterMark(NULL);

        if (reste < stackMin2)
        {
        stackMin2 = reste;
        Serial.printf("pollVMTask %i\n", stackMin2);
        }
#endif
        if(client.httpGETRequest("/pull", &pullBody))
        {
            if (pullBody.length() > 0)
                strips.apply(pullBody);
        }
        else
            errorHandler(HTTP_GET_FAILED);

        vTaskDelay(VM_POLLING_RATE / portTICK_PERIOD_MS);
    }
}

void pushVMTask(void * parameter)
{
    for(;;)
    {
#ifdef DEBUG_STACK
        int reste = uxTaskGetStackHighWaterMark(NULL);

        if (reste < stackMin3)
        {
            stackMin3 = reste;
            Serial.printf("pushVMTask %i\n", stackMin3);
        }
#endif
        if(strips.getCurrentURI(&pushURI))
            if(!client.httpPOSTRequest(pushURI, ""))
                errorHandler(HTTP_POST_FAILED);

        if(macros.getCurrentURI(&pushURI))
            if(!client.httpPOSTRequest(pushURI, ""))
                errorHandler(HTTP_POST_FAILED);

        vTaskDelay(VM_PUSHING_RATE / portTICK_PERIOD_MS);
    }
}

void pollHardwareTask(void * parameter)
{
    for(;;)
    {
#ifdef DEBUG_STACK
        int reste = uxTaskGetStackHighWaterMark(NULL);

        if (reste < stackMin1)
        {
            stackMin1 = reste;
            Serial.printf("pollHardwareTask %i\n", stackMin1);
        }
#endif
        strips.update();
        macros.update();
        vTaskDelay(HARDWARE_POLLING_RATE / portTICK_PERIOD_MS);
    }
}

void errorHandlingTask(void * parameter)
{
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

        if (errorId)
        {
            switch (errorId)
            {
            case HTTP_POST_FAILED:
                break;

            case HTTP_GET_FAILED:
                break;
            
            default:
                break;
            }
        }

        vTaskDelay(ERROR_HANDLING_RATE / portTICK_PERIOD_MS);
    }
}






bool connectWifi()
{
    IPAddress staticIP(ST_IP);
    IPAddress gateway(ST_GATEWAY);
    IPAddress subnet(ST_NETMASK);

    WiFi.config(staticIP, gateway, subnet);
    WiFi.setHostname(HOST_NAME);
    

    WiFi.begin(ssid, password);
    delay(WIFI_CONNECT_TIMEOUT);
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.disconnect();
        return false;
    }

#ifdef DEBUG
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
#endif

    return true;
}

void setupOTA()
{
    ArduinoOTA.setHostname(HOST_NAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);

    ArduinoOTA
        .onStart([]() {
            Serial.println("Starting OTA");
#ifdef DEBUG
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
                type = "sketch";
            else
                type = "filesystem";
            Serial.println("Start updating " + type);
#endif
        })
        .onEnd([]() {
            Serial.println("\nEnd OTA");
        })
        .onProgress([](unsigned int progress, unsigned int total) {
#ifdef DEBUG
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
#endif
        })
        .onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
            else if (error == OTA_END_ERROR) Serial.println("End Failed");
        });

    ArduinoOTA.begin();
}

void errorHandler(int code)
{
    Serial.printf("Error %i\n", code);
    statusLed.apply(HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    statusLed.apply(LOW);
}