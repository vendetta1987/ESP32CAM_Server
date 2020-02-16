#ifndef WIFI_CREDENTIALS_H
#define WIFI_CREDENTIALS_H

#include <WiFi.h>

struct WifiCredentials
{
    WifiCredentials(char const *SSID, char const *pass);

    char const *const SSID;
    char const *const password;
};

WifiCredentials const *const getClosestWifi(WiFiClass &wifiCl);

extern const unsigned char knownCredsCnt;
extern WifiCredentials const *const knownCreds[];

#endif
