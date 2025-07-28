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
const char* subscribe_topics[NB_SUB_TOPICS] = {"macrodeck/vm", "macrodeck/hb"};
const char* gains_topic = "macrodeck/gains";
const char* mutes_topic = "macrodeck/mutes";
const char* macro_topic = "macrodeck/macros";

// Vars
static uint16_t fatalError = OK;
static double stripGains[NB_HARDWARE_STRIPS];
static uint8_t stripMutes[NB_HARDWARE_STRIPS];
static uint8_t macros;
static String in_msg;
static double in_gains[NB_HARDWARE_STRIPS];
static unsigned long last_hb_time;

// ========== Functions ==========
static uint16_t connectWifi();
static void setupOTA();
static void errorNotify(uint16_t code);
static void shutdown();

static void mqtt_in(char* topic, byte* payload, unsigned int length);

// ===== Tasks =====
static void pollHardwareTask(void * parameter);
static void pollMqttTask(void * parameter);
static void pushVMTask(void * parameter);
static void pushMacro(void * parameter);
static void errorHandlingTask(void * parameter);

static TaskFunction_t taskFct[NB_TASKS] = { pollHardwareTask, pollMqttTask, pushVMTask, pushMacro, errorHandlingTask };
static char* taskNames[NB_TASKS] = { "Harware", "MQTT", "Slider", "Macro", "Errors" };
static int stacks[NB_TASKS] = { 3000, 2500, 3000, 3000, 1500 };
static int taskPriorities[NB_TASKS] = { 3, 4, 2, 1, 5 };
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

            ret = client.init(mqtt_broker, mqtt_port, subscribe_topics, NB_SUB_TOPICS, mqtt_in);
            
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

    xTaskCreate(taskFct[4], taskNames[4], stacks[4], NULL, 1, &taskHandles[4]);
    last_hb_time = millis();
}

void loop()
{
    ArduinoOTA.handle();
    if (millis() - last_hb_time > TIME_BEFORE_SLEEP_MS)
        shutdown();
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
            xTaskNotify(taskHandles[4], ret, eSetValueWithOverwrite);
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

        if (stripModule.getMutes(stripMutes) == OK)
        {
            for (int i = 0; i < NB_HARDWARE_STRIPS; ++i)
            {
                if (stripMutes[i] != NO_MUTE_UPDATE)
                client.pub(mutes_topic, String(i).c_str());
            }
        }
            

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
                // Do nothing
                break;
            case WARNING:
            case ERROR:
                errorNotify(code);
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
    // HB first
    if (strcmp(topic, subscribe_topics[1]) == 0)
    {
        statusLed.toggle();
        last_hb_time = millis();
    }
    else if (strcmp(topic, subscribe_topics[0]) == 0)
    {
        in_msg = "";
        for (int i = 0; i < length; i++) 
            in_msg += (char) payload[i];

        uint8_t id, gain;
        bool mute;

        int colonPos = in_msg.indexOf(':');
        int commaPos = in_msg.indexOf(',');

        if (colonPos != -1 && commaPos != -1 && colonPos < commaPos)
        {
            id = in_msg.substring(0, colonPos).toInt();
            gain = in_msg.substring(colonPos + 1, commaPos).toInt();
            mute = in_msg.substring(commaPos + 1).toInt();

            // Nothing with gains currently
            stripModule.apply(id, gain, mute);
        }
        else 
        {
            xTaskNotify(taskHandles[4], MQTT_BAD_IN_MSG, eSetValueWithOverwrite);
        }
    }
    else
    {
        xTaskNotify(taskHandles[4], INVALID_MQTT_TOPIC, eSetValueWithOverwrite);
    }
}

uint16_t connectWifi()
{
    IPAddress staticIP(ST_IP);
    IPAddress gateway(ST_GATEWAY);
    IPAddress subnet(ST_NETMASK);
    IPAddress dns(ST_DNS);

    WiFi.config(staticIP, gateway, subnet, dns);
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
    for (uint8_t i = 0; i < 5; i++)
        stripModule.apply(i, 0, (code >> i) & 1);

    vTaskDelay(500 / portTICK_PERIOD_MS);

    for (uint8_t i = 0; i < 5; i++)
        stripModule.apply(i, 0, 0);
}

void shutdown()
{
    statusLed.apply(false);

#ifdef DEBUG
    Serial.println("Shutting down! DODO");
    Serial.println(millis());
    Serial.println(last_hb_time);
#endif

    for (uint8_t i = 0; i < NB_TASKS - 1; ++i)
        vTaskSuspend(taskHandles[i]);

    client.disconnect();
    esp_deep_sleep_start();
}