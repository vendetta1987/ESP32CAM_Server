#ifndef DHT22SENSOR_H
#define DHT22SENSOR_H

#include <SimpleDHT.h>

void getDHTReadings(float &temp, float &hum);

const unsigned int DHT_PIN = 2;
extern SimpleDHT22 dht22;

#endif