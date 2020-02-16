#include "WifiCredentials.h"
#include <stdint.h>
#include <WString.h>
#include <HardwareSerial.h>

WifiCredentials::WifiCredentials(char const *SSID, char const *pass) : SSID(SSID), password(pass)
{
}

WifiCredentials const *const getClosestWifi(WiFiClass &wifiCl)
{
    uint8_t wifiCnt = wifiCl.scanNetworks(false, true);
    uint8_t maxRSSIIdx = -1;
    int32_t maxRSSI = -512;

    for (uint8_t i = 0; i < wifiCnt; i++)
    {
        String ssid = wifiCl.SSID(i);
        Serial.print("found " + ssid + "\n");

        for (uint8_t j = 0; j < knownCredsCnt; j++)
        {
            if (strcmp(ssid.c_str(), knownCreds[j]->SSID) == 0)
            {
                int32_t rssi = wifiCl.RSSI(i);
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


const WifiCredentials fbCreds = {"SSID1", "password"};

WifiCredentials const *const knownCreds[] = {&wifiCreds};
const unsigned char knownCredsCnt = sizeof(knownCreds) / sizeof(*knownCreds);
