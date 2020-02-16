#include "DHT22Sensor.h"

void getDHTReadings(SimpleDHT22 *const sensor, float &temp, float &hum)
{
    int err = sensor->read2(&temp, &hum, NULL);

    if (err != SimpleDHTErrSuccess)
    {
        Serial.print("Read DHT22 failed, err=");
        Serial.println(err);
    }
}
