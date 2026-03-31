#include "WString.h"
#include "WifiAP.h"
#include "WebPage.h"

const byte DNS_PORT = 53;

WifiAPController::WifiAPController() : server(80), webSocket(81) {};

WifiAPController::~WifiAPController() {};

void WifiAPController::loadID(String idPadrao) {
  EEPROM.begin(512);
  
  String currentTime = String(__DATE__) + " " + String(__TIME__);
  
  String savedTime = "";
  for (int i = 64; i < 100; ++i) {
    char c = char(EEPROM.read(i));
    if (c == '\0' || c == 255) break;
    savedTime += c;
  }

  if (savedTime != currentTime) {
    for (unsigned int i = 0; i < currentTime.length(); ++i) {
      EEPROM.write(64 + i, currentTime[i]);
    }
    EEPROM.write(64 + currentTime.length(), '\0');
    EEPROM.commit();
    
    this->saveID(idPadrao);
    this->id = idPadrao;
  } 
  else {
    this->id = "";
    for (int i = 0; i < 32; ++i) {
      char c = char(EEPROM.read(i));
      if (c == '\0' || c == 255) break;
      this->id += c;
    }
    
    if (this->id.length() < 2) {
      this->id = idPadrao; 
    }
  }
}

void WifiAPController::saveID(String newID) {
  id = newID;
  for (unsigned int i = 0; i < newID.length() && i < 31; ++i) {
    EEPROM.write(i, newID[i]);
  }
  EEPROM.write(newID.length(), '\0');
  EEPROM.commit();
}

void WifiAPController::onWsEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED: break;
    case WStype_CONNECTED: break;
    case WStype_TEXT:
      Serial.println((char*)payload); 
      break;
    case WStype_BIN: break;
  }
}

void WifiAPController::startAP() {
  WiFi.mode(WIFI_AP);
  
  String redeAP = "Controle-" + id;
  
  WiFi.softAP(redeAP.c_str(), "", 1, 0, 1); 
  IPAddress apIP = WiFi.softAPIP(); 

  dnsServer.start(DNS_PORT, "*", apIP);

  server.on("/", [this]() {
    String html = String(FPSTR(MAIN_page));

    this->server.send(200, "text/html", html);
  });

  server.on("/config", [this]() {
    String html = String(FPSTR(CONFIG_page));
    
    html.replace("{{CURRENT_ID}}", this->id);
    
    this->server.send(200, "text/html", html);
  });

  server.on("/salvar_config", HTTP_POST, [this]() {
    if (this->server.hasArg("novo_id")) {
      String novoID = this->server.arg("novo_id");
      novoID.trim();
      novoID.toUpperCase(); 
      
      this->saveID(novoID);
      
      String html = String(FPSTR(SAVE_CONFIG_page));
      
      html.replace("{{NEW_ID}}", novoID);
      
      this->server.send(200, "text/html", html);

      delay(2000); 
      
      WiFi.disconnect(true);
      delay(500); 
      
      ESP.restart();
    } else {
      this->server.send(400, "text/plain", "Erro: ID nao fornecido");
    }
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