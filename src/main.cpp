#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>

#include <freertos/FreeRTOS.h>

#include "constants.h"
#include "config.h"

#include "Buttons.h"
#include "Sliders.h"
#include "ClientHttp.h"
#include "StripModule.h"
#include "utils.h"

// ===== Macro cb =====

static void muteCallback(const int id);

// ===== Tasks =====

static void pollVMTask(void * parameter);
static void pushVMTask(void * parameter);
static void pollStripsTask(void * parameter);

// Strips modules
static StripModule strips[NB_HARDWARE_STRIPS];
static bool muteButtonStates[NB_HARDWARE_STRIPS];

// HTTP Client
static ClientHttp client(serverIP, serverPort);




// Functions
static void connectWifi();
static void setupOTA();

void setup()
{
  Serial.begin(115200);

  connectWifi();
  setupOTA();

  for (int i = 0; i < NB_HARDWARE_STRIPS; ++i)
    strips[i].init(vmStripID[i], sliderPins[i], muteButtonPins[i], muteLedPins[i]);
    
  //xTaskCreate(pollStripsTask, "Poll Strips", 2500, NULL, 1, NULL);
  //xTaskCreate(pollVMTask, "Poll VM", 3000, NULL, 1, NULL);
  //xTaskCreate(pushVMTask, "Push VM", 3000, NULL, 1, NULL);
}

void loop()
{
  ArduinoOTA.handle(); // TODO : Add switch to allow OTA 
}

void pollVMTask(void * parameter)
{
  for(;;)
  {
#ifdef DEBUG_STACK
    Serial.printf("pollVM %i\n", uxTaskGetStackHighWaterMark(NULL));
#endif

    for (int i = 0; i < NB_HARDWARE_STRIPS; ++i)
    {
      String resp;
      if(client.httpGETRequest(strips[i].getPullURI(), &resp))
        strips[i].apply(resp);
    }

    vTaskDelay(VM_POLLING_RATE / portTICK_PERIOD_MS);
  }
}

void pushVMTask(void * parameter)
{
  for(;;)
  {
#ifdef DEBUG_STACK
    Serial.printf("pushVM %i\n", uxTaskGetStackHighWaterMark(NULL));
#endif

    for (int i = 0; i < NB_HARDWARE_STRIPS; ++i)
    {
      String uri;
      if(strips[i].getPushURI(&uri))
      {
        if(!client.httpPOSTRequest(uri, ""))
          Serial.printf("Sadge %i\n", i);
      }
    }

    vTaskDelay(VM_PUSHING_RATE / portTICK_PERIOD_MS);
  }
}

void pollStripsTask(void * parameter)
{
  for(;;)
  {
#ifdef DEBUG_STACK
    Serial.printf("pollSlidersTask %i\n", uxTaskGetStackHighWaterMark(NULL));
#endif
    for (int i = 0; i < NB_HARDWARE_STRIPS; ++i)
      strips[i].update();

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