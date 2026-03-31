#ifndef WIFIAP_H
#define WIFIAP_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <DNSServer.h>

class WifiAPController {
private:
  ESP8266WebServer server;
  WebSocketsServer webSocket;
  DNSServer dnsServer; // Instância do Servidor DNS
  
  static void onWsEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);

public:
  WifiAPController();
  ~WifiAPController();

  void startAP(const char* ssid, const char* password);
  void handleServers();
};

#endif