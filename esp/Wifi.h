#ifndef WIFI_H
#define WIFI_H

#include <ESP8266WiFi.h> // WiFi
#include <ESP8266mDNS.h> // multi DNS
#include <WiFiManager.h>

class WifiController
{
public:
  WifiController(); // Constructor
  ~WifiController();

  String ip = "";

  void startWiFi();
  void startMDNS(String mdnsName);
  void updateMDNS();
};

#endif
