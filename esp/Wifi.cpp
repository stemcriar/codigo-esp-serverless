#include "Wifi.h"

WifiManager::WifiManager()
{                                                     // Constructor
  wifiMulti.addAP("STEM-MAKERSPACE", "St3mMK2k24!!"); // adicionar credenciais das redes
};

WifiManager::~WifiManager() { /* ¯\_(ツ)_/¯ */ }; // Destructor

void WifiManager::startWiFi()
{

  Serial.println("\n\rConnecting");
  while (wifiMulti.run() != WL_CONNECTED)
  { // Esperar WiFi conectar
    delay(250);
    Serial.print(".");
  };

  Serial.print("\r\n");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID()); // Nome da rede
  Serial.print("IP address:\t");
  Serial.print(WiFi.localIP()); // Ip do esp na rede local

  this->ip = WiFi.localIP().toString().c_str();

  Serial.println("\r\n");
};

void WifiManager::startMDNS(String mdnsName)
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

void WifiManager::updateMDNS()
{
  MDNS.update();
};
