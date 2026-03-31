#ifndef WIFIAP_H
#define WIFIAP_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <DNSServer.h>
#include <EEPROM.h> 

class WifiAPController {
private:
  ESP8266WebServer server;
  WebSocketsServer webSocket;
  DNSServer dnsServer;
  String id; 
  
  static void onWsEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);

public:
  WifiAPController();
  ~WifiAPController();

  void loadID(String idPadrao);
  void saveID(String newID);

  void startAP(); 
  void handleServers();
};

#endif