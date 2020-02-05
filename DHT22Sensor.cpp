#include "DHT22Sensor.h"

void getDHTReadings(float &temp, float &hum)
{
    int err = dht22.read2(&temp, &hum, NULL);

    if (err != SimpleDHTErrSuccess)
    {
        Serial.print("Read DHT22 failed, err=");
        Serial.println(err);
    }
}

SimpleDHT22 dht22(DHT_PIN);