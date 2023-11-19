#ifndef CLIENT_WS_H_
#define CLIENT_WS_H_

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "ErrorCode.h"
#include "Constants.h"

typedef std::function<void(char* topic, byte* payload, unsigned int length)> MQTTCallback;

class ClientMQTT
{
private:
    WiFiClient espClient;
    PubSubClient client;
        
public:
    ClientMQTT();
    uint16_t init(const char* ip, int port, const char** topics, const int nb_topic, MQTTCallback cb);
    uint16_t pub(const char* topic, const char* data);
    void disconnect();
    void loop();
};

#endif