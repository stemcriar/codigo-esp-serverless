#include "WifiAP.h"

WifiAPController wifi;

// Nome da rede que a ESP vai emitir
const char* ssid = "CAR003";
const char* password = ""; // Deixe vazio para rede aberta

void setup() {
  // A velocidade DEVE ser a mesma configurada no código do Arduino!
  Serial.begin(9600);
  delay(1000);
  
  // Envia handshakes de segurança para destravar o "serial.waitHandshake" do Arduino, caso ele exista
  Serial.println("{\"from\":\"ESP\",\"state\":\"OK\"}");
  Serial.println("{\"ESP\":\"Hello\"}"); 
  
  // Inicia a rede Wi-Fi Standalone e os Servidores
  wifi.startAP(ssid, password);
}

void loop() {
  // Mantém tudo a rodar perfeitamente
  wifi.handleServers();
}