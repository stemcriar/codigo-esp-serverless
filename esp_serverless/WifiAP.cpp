#include "WifiAP.h"
#include "WebPage.h"

const byte DNS_PORT = 53;

WifiAPController::WifiAPController() : server(80), webSocket(81) {};

WifiAPController::~WifiAPController() {};

void WifiAPController::onWsEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      break;
    case WStype_CONNECTED:
      break;
    case WStype_TEXT:
      // Envia o JSON recebido exatamente como chegou para o Arduino
      Serial.println((char*)payload); 
      break;
    case WStype_BIN:
      break;
  }
}

void WifiAPController::startAP(const char* ssid, const char* password) {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password, 1, 0, 1); 
  
  IPAddress apIP = WiFi.softAPIP();
  dnsServer.start(DNS_PORT, "*", apIP);

  server.on("/", [this]() {
    this->server.send(200, "text/html", MAIN_page);
  });

  server.onNotFound([this]() {
    String ipAtual = WiFi.softAPIP().toString(); 
    
    this->server.sendHeader("Location", "http://" + ipAtual + "/", true);
    this->server.send(302, "text/plain", "");
  });

  server.begin();
  
  webSocket.begin();
  webSocket.onEvent(onWsEvent);
}

void WifiAPController::handleServers() {
  dnsServer.processNextRequest(); 
  server.handleClient();
  webSocket.loop();
}