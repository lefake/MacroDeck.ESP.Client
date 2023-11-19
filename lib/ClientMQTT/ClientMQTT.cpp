#include "ClientMQTT.h"

ClientMQTT::ClientMQTT() : client(espClient) {}

uint16_t ClientMQTT::init(const char* ip, int port, const char** topics, const int nb_topic, MQTTCallback cb)
{
    uint16_t ret = MQTT_TIMEOUT;

    client.setServer(ip, port);
    client.setCallback(cb);

    unsigned long start = millis();

    while (!client.connected() && (millis() - start < MQTT_CONNECT_TIMEOUT))
    {
        if (client.connect("ESP32Client"))
        {
            for (int i = 0; i < nb_topic; ++i)
                client.subscribe(topics[i]);

            ret = OK;
        }
    }

    return ret;
}

uint16_t ClientMQTT::pub(const char* topic, const char* data)
{
    return client.publish(topic, data) ? OK : MQTT_PUB_ERROR;
}

void ClientMQTT::disconnect()
{
    client.disconnect();
}

void ClientMQTT::loop()
{
    client.loop();
}