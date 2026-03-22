#ifndef WIFI_H
#define WIFI_H

#include <ESP8266WiFi.h> // WiFi
#include <ESP8266mDNS.h> // multi DNS
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

class WifiController
{
private:
  ESP8266WebServer server;
  String id;

public:
  WifiController(); // Constructor
  ~WifiController();

  String ip = "";
  String serverIP = "";

  void startWiFi(String id);
  void startMDNS(String mdnsName);
  void updateMDNS();
  
  void startWebServer(String id);
  void handleWebServer();

  void loadServerIP();
  void saveServerIP(String newIP);
};

#endif
