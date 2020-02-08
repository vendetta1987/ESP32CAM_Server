#ifndef DHT22SENSOR_H
#define DHT22SENSOR_H

#include <SimpleDHT.h>

const unsigned int DHT_PIN = 2;

void getDHTReadings(SimpleDHT22 *const sensor, float &temp, float &hum);

#endif