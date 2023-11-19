#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

#include "Constants.h"
#include "config.h"
#include "pins.h"
#include "ErrorCode.h"

#include "ClientMQTT.h"
#include "StripModule.h"
#include "MacroModule.h"

// Modules
static StripModule stripModule;
static MacroModule macroModule;
static Led statusLed;

// MQTT
static ClientMQTT client;
const char* subscribe_topics[] = {"macrodeck/vm",};
const char* gains_topic = "macrodeck/gains";
const char* mutes_topic = "macrodeck/mutes";
const char* macro_topic = "macrodeck/macros";

// Vars
static uint16_t fatalError = OK;
static double stripGains[NB_HARDWARE_STRIPS];
static uint8_t stripMutes;
static uint8_t macros;
static String in_msg;
static double in_gains[NB_HARDWARE_STRIPS];

// ========== Functions ==========
static uint16_t connectWifi();
static void setupOTA();
static void errorNotify(uint16_t code);

static void mqtt_in(char* topic, byte* payload, unsigned int length);

// ===== Tasks =====
static void pollHardwareTask(void * parameter);
static void pollMqttTask(void * parameter);
static void pushVMTask(void * parameter);
static void pushMacro(void * parameter);
static void errorHandlingTask(void * parameter);

static TaskFunction_t taskFct[NB_TASKS] = { pollHardwareTask, pollMqttTask, pushVMTask, pushMacro, errorHandlingTask };
static char* taskNames[NB_TASKS] = { "A", "B", "C", "D", "E" };
static int stacks[NB_TASKS] = { 5000, 5000, 5000, 5000, 5000 };
static int taskPriorities[NB_TASKS] = { 1, 1, 2, 2, 1 };
static TaskHandle_t taskHandles[NB_TASKS];
static bool taskRun[NB_TASKS] = { true, true, true, true, true };

void setup()
{
    Serial.begin(115200);
    esp_sleep_enable_timer_wakeup(SLEEP_TIME_uS);

    uint16_t ret = statusLed.init(statusLedPin);
    ret |= stripModule.init(sliderPins, muteButtonPins, muteLedPins, NB_HARDWARE_STRIPS);
    ret |= macroModule.init(muxSelectPins, muxInputPin, NB_HARDWARE_MACRO);

    if (GET_SEVERITY(ret) == SUCCESS)
    {
        ret = connectWifi();
        if (ret == OK)
        {
            setupOTA();

            ret = client.init(mqtt_broker, mqtt_port, subscribe_topics, 1, mqtt_in);
            
            if (ret == OK)
            {
                Serial.println("Starting");
                for (uint8_t i = 0; i < NB_TASKS - 1; ++i)
                    xTaskCreate(taskFct[i], taskNames[i], stacks[i], NULL, taskPriorities[i], &taskHandles[i]);
            }
            else
                fatalError = MQTT_INIT_FAILED;
        }
        else
            fatalError = WIFI_NOT_CONNECTED;
    }
    else
        fatalError = INIT_FAILED;

    xTaskCreate(taskFct[3], taskNames[3], stacks[3], NULL, 1, &taskHandles[3]);
}

void loop()
{
    ArduinoOTA.handle();
}

// ==================== Tasks ====================

void pollHardwareTask(void * parameter)
{
    uint8_t task_id = 0;
    uint16_t ret;
    while(taskRun[task_id])
    {
#ifdef DEBUG_STACK
        int reste = uxTaskGetStackHighWaterMark(NULL);

        if (reste < stacks[task_id])
        {
            stacks[task_id] = reste;
            Serial.printf("pollHardwareTask %i\n", stacks[task_id]);
        }
#endif
        ret = stripModule.update();
        if (GET_SEVERITY(ret) == SUCCESS)
            ret = macroModule.update();

        if (GET_SEVERITY(ret) != SUCCESS)
            xTaskNotify(taskHandles[3], ret, eSetValueWithOverwrite);
        vTaskDelay(HARDWARE_POLLING_RATE / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void pushVMTask(void * parameter)
{
    uint8_t task_id = 2;
    
    while(taskRun[task_id])
    {
#ifdef DEBUG_STACK
        int reste = uxTaskGetStackHighWaterMark(NULL);

        if (reste < stacks[task_id])
        {
            stacks[task_id] = reste;
            Serial.printf("pushVMTask %i\n", stacks[task_id]);
        }
#endif
        if (stripModule.getGains(stripGains) == OK)
        {
            for (int i = 0; i < NB_HARDWARE_STRIPS; ++i)
            {
                if (stripGains[i] != GAIN_VALUE_NOT_UPDATED)
                    client.pub(gains_topic, (String(i) + ":" + String(stripGains[i])).c_str());
            }
        }

        if (stripModule.getMutes(&stripMutes) == OK)
            client.pub(mutes_topic, String(stripMutes).c_str());

        vTaskDelay(VM_PUSHING_RATE / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void pollMqttTask(void * parameter)
{
    uint8_t task_id = 1;
    uint16_t ret;
    while(taskRun[task_id])
    {
#ifdef DEBUG_STACK
        int reste = uxTaskGetStackHighWaterMark(NULL);

        if (reste < stacks[task_id])
        {
            stacks[task_id] = reste;
            Serial.printf("pollMqttTask %i\n", stacks[task_id]);
        }
#endif
        client.loop();
        vTaskDelay(MQTT_POLLING_RATE / portTICK_PERIOD_MS);
    }
}

void pushMacro(void * parameter)
{
    uint8_t task_id = 3;
    
    while(taskRun[task_id])
    {
#ifdef DEBUG_STACK
        int reste = uxTaskGetStackHighWaterMark(NULL);

        if (reste < stacks[task_id])
        {
            stacks[task_id] = reste;
            Serial.printf("pushMacro %i\n", stacks[task_id]);
        }
#endif
        if (macroModule.getMacros(&macros) == OK)
            client.pub(macro_topic, String(macros).c_str());

        vTaskDelay(MACRO_PUSHING_RATE / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void errorHandlingTask(void * parameter)
{
    uint32_t ulNotificationValue;
    uint16_t errorCode;
    uint16_t severity;
    uint16_t code;
    uint8_t task_id = 4;

    for(;;)
    {
#ifdef DEBUG_STACK
        int reste = uxTaskGetStackHighWaterMark(NULL);

        if (reste < stacks[task_id])
        {
            stacks[task_id] = reste;
            Serial.printf("errorHandlingTask %i\n", stacks[task_id]);
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

            Serial.println(errorCode);

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
                    for (uint8_t i = 0; i < NB_TASKS - 1; ++i)
                        vTaskSuspend(taskHandles[i]);

                    client.disconnect();
                    esp_deep_sleep_start();
                }
                break;
            case FATAL:
            default:
                for (uint8_t i = 0; i < NB_TASKS - 1; ++i)
                    taskRun[i] = false;

                fatalError = errorCode;
                break;
            }
        }
    }
}

// ==================== Private ====================

void mqtt_in(char* topic, byte* payload, unsigned int length)
{
    if (strcmp(topic, subscribe_topics[0]) == 0)
    {
        in_msg = "";
        for (int i = 0; i < length; i++) 
            in_msg += (char) payload[i];

        stripModule.apply(in_gains, in_msg.toInt());
    }
}

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