#include "MQTT.h"

int MyMQTTClient::connect(const char *host, uint16_t port)
{
    return MqttClient::connect(host, port);
}

bool MyMQTTClient::connectToBroker()
{
    return !connect(_brokerAddress);
}

void MyMQTTClient::sendMessage(char const *const topic, char const *const message)
{
    Serial.printf("sending message:\"%s\" to topic:\"%s\"\n", message, topic);

    beginMessage(topic);
    this->print(message);
    endMessage();
}
