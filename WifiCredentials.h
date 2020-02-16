#ifndef WIFI_CREDENTIALS
#define WIFI_CREDENTIALS

#include <WiFi.h>

struct WifiCredentials
{
    WifiCredentials(char const *SSID, char const *pass);

    char const *const SSID;
    char const *const password;
};

WifiCredentials const *const getClosestWifi(WiFiClass &wifiCl);

extern const unsigned int knwonCredsCnt;
extern WifiCredentials const *const knownCreds[];

#endif