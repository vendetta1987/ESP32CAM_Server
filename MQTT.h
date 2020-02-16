#ifndef MQTT_H
#define MQTT_H

#include <ArduinoMqttClient.h>
#include <MqttClient.h>
#include <WiFi.h>

//class is needed to fix bugs in ESP32 implementation
//https://github.com/arduino-libraries/ArduinoMqttClient/issues/5
class MyMQTTClient : public MqttClient
{
public:
    MyMQTTClient(char const *const brokerAddress = "default_broker") : MqttClient(_wifiClient), _brokerAddress(brokerAddress){};
    virtual ~MyMQTTClient(){};

    bool connectToBroker();

    //exact copy of MqttClient.connect which can't be found otherwise, weird
    virtual int connect(const char *host, uint16_t port = 1883);
    //pure virtual methods which are not implemented anywhere otherwise and thus prevent instantiation
    virtual int connect(IPAddress ip, uint16_t port, int timeout){};
    virtual int connect(const char *host, uint16_t port, int timeout){};

private:
    WiFiClient _wifiClient;
    char const *const _brokerAddress;
};

#endif