#include "Wifi.h"

WifiController::WifiController() {};

WifiController::~WifiController() { /* ¯\_(ツ)_/¯ */ }; // Destructor

void WifiController::startWiFi()
{
  Serial.println("\n\rConnecting");
  WiFiManager wifiManager;

  delay(500);
  if (!wifiManager.autoConnect("Config-ESP-CRIAR")) {
    Serial.println("Falha ao ligar. A reiniciar...");
    ESP.restart();
    delay(1000);
  }

  Serial.print("\r\n");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID()); // Nome da rede
  Serial.print("IP address:\t");
  Serial.print(WiFi.localIP()); // Ip do esp na rede local

  this->ip = WiFi.localIP().toString().c_str();

  Serial.println("\r\n");
};

void WifiController::startMDNS(String mdnsName)
{ // Iniciar o mDNS com o nome desejado para a rede .local

  MDNS.begin(mdnsName); // começa a transmissão do nome
  if (!MDNS.begin(mdnsName))
  {
    Serial.println("Error setting up MDNS responder!");
  }
  MDNS.addService("ws", "tcp", 81);
  Serial.print("mDNS responder started: http://");
  Serial.print(mdnsName);
  Serial.println(".local");
};

void WifiController::updateMDNS()
{
  MDNS.update();
};
