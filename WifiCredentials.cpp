#include "WifiCredentials.h"
#include <stdint.h>
#include <WString.h>
#include <HardwareSerial.h>

WifiCredentials::WifiCredentials(char const *SSID, char const *pass) : SSID(SSID), password(pass)
{
}

WifiCredentials const *const getClosestWifi(WiFiClass &wifiCl)
{
    int16_t wifiCnt = wifiCl.scanNetworks(false, true);
    int16_t maxRSSIIdx = -1;
    int8_t maxRSSI = -128;

    for (size_t i = 0; i < wifiCnt; i++)
    {
        String ssid = wifiCl.SSID(i);
        Serial.print("found " + ssid + "\n");

        for (size_t j = 0; j < knwonCredsCnt; j++)
        {
            if (strcmp(ssid.c_str(), knownCreds[j]->SSID) == 0)
            {
                int8_t rssi = wifiCl.RSSI(i);
                Serial.print(ssid + " RSSI=" + String(rssi) + "\n");

                if (rssi > maxRSSI)
                {
                    maxRSSI = rssi;
                    maxRSSIIdx = j;
                }
            }
        }
    }

    Serial.print("choosing " + String(knownCreds[maxRSSIIdx]->SSID));
    return knownCreds[maxRSSIIdx];
}

char const *const password = "password";

const WifiCredentials fbCreds = {"SSID1", password};
const WifiCredentials rpCreds = {"SSID2", password};

WifiCredentials const *const knownCreds[] = {&fbCreds, &rpCreds};
const unsigned int knwonCredsCnt = sizeof(knownCreds) / sizeof(*knownCreds);