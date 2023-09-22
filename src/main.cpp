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

// ===== Tasks =====

static void pollVMTask(void * parameter);
static void pushVMTask(void * parameter);
static void pollHardwareTask(void * parameter);

// Modules
static StripModule strips;
static MacroModule macros;

// HTTP Client
static ClientHttp client(serverIP, serverPort);

static String pullBody;
static String pushURI;

static int stackMin1 = 5000;
static int stackMin2 = 5000;
static int stackMin3 = 5000;

// Functions
static void connectWifi();
static void setupOTA();
static void errorHandler(int code);

void setup()
{
    Serial.begin(115200);

    connectWifi();
    setupOTA();

    strips.init(sliderPins, muteButtonPins, muteLedPins, NB_HARDWARE_STRIPS);
    macros.init(macroButtonPins, NB_HARDWARE_MACRO);

    Serial.println("Sadge");

    xTaskCreate(pollHardwareTask, "Poll Hardware", 5000, NULL, 1, NULL);
    xTaskCreate(pollVMTask, "Poll VM", 5000, NULL, 1, NULL);
    xTaskCreate(pushVMTask, "Push VM", 5000, NULL, 1, NULL);
}

void loop()
{
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
        vTaskDelay(SLIDERS_POLLING_RATE / portTICK_PERIOD_MS);
    }
}

void connectWifi()
{
    IPAddress staticIP(ST_IP);
    IPAddress gateway(ST_GATEWAY);
    IPAddress subnet(ST_NETMASK);

    WiFi.config(staticIP, gateway, subnet);
    WiFi.setHostname(HOST_NAME);

    WiFi.begin(ssid, password);
#ifdef DEBUG
    Serial.println("Connecting");
#endif
    // TODO : Timeout
    while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
#ifdef DEBUG
    Serial.print(".");
#endif
    }
#ifdef DEBUG
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
#endif
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
}