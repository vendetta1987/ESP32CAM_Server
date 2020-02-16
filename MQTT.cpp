#include "MQTT.h"

int MyMQTTClient::connect(const char *host, uint16_t port)
{
    return MqttClient::connect(host, port);
}

bool MyMQTTClient::connectToBroker()
{
    return !connect(_brokerAddress);
}
