#include "WifiAP.h"
#include "WebPage.h"

// Inicializa servidores (Porta 80 para Web, Porta 81 para WebSocket)
WifiAPController::WifiAPController() : server(80), webSocket(81) {};

WifiAPController::~WifiAPController() {};

// Implementação do gerenciador de eventos WebSocket
void WifiAPController::onWsEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      // Cliente desconectou
      break;
    case WStype_CONNECTED:
      // Cliente conectou (pode enviar um ping de confirmação se quiser)
      break;
    case WStype_TEXT:
      // RECEBEMOS DADOS DO JOYSTICK!
      // payload contém o JSON pronto: {"from": "X", "state": "Y"}
      
      // Repassamos EXATAMENTE como recebemos para o Arduino via Serial
      Serial.println((char*)payload); 
      break;
    case WStype_BIN:
      // Não utilizamos dados binários
      break;
  }
}

void WifiAPController::startAP(const char* ssid, const char* password) {
  // 1. Configura o modo Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password); // Cria a rede
  
  // O IP padrão do modo AP da ESP é 192.168.4.1

  // 2. Configura o Servidor Web (HTTP)
  // Quando acessar http://192.168.4.1/, entrega a página do joystick
  server.on("/", [this]() {
    this->server.send(200, "text/html", MAIN_page);
  });
  server.begin();
  
  // 3. Configura o Servidor WebSocket
  webSocket.begin();
  // Vincula a função de evento (usando um wrapper estático/lambda para acessar o callback)
  webSocket.onEvent(onWsEvent);
}

void WifiAPController::handleServers() {
  server.handleClient(); // Processa requisições web
  webSocket.loop();      // Processa dados websocket
}