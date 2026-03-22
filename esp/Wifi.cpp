#include "Wifi.h"

WifiController::WifiController() : server(80) {};

WifiController::~WifiController() { /* ¯\_(ツ)_/¯ */ }; // Destructor

void WifiController::startWiFi(String id)
{
  Serial.println("\n\rConnecting");
  WiFiManager wifiManager;

  String apName = "Config-ESP-" + id;

  delay(500);
  if (!wifiManager.autoConnect(apName.c_str())) {
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

void WifiController::startWebServer(String id) 
{
  this->id = id;

  server.on("/", [this]() {
    String html = "<html><head><meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"; 
    html += "<style>body{font-family:sans-serif; text-align:center; padding-top:50px;} input, button{padding:10px; font-size:16px; margin:5px;} button{cursor:pointer; background-color:#4CAF50; color:white; border:none; border-radius:5px;}</style>";
    html += "</head><body>";
    html += "<h1>Configurações: " + this->id + "</h1>";
    html += "<p>O IP atual do <strong>Servidor STEM Criar</strong> é: <br><strong>" + this->serverIP + "</strong></p>";
    
    html += "<form action='/salvar_ip' method='POST'>";
    html += "<input type='text' name='novo_ip' placeholder='Digite o novo IP' required>";
    html += "<button type='submit'>Salvar IP</button>";
    html += "</form>";
    html += "</body></html>";
    
    this->server.send(200, "text/html", html);
  });

  server.on("/salvar_ip", HTTP_POST, [this]() {
    if (this->server.hasArg("novo_ip")) {
      String novoIP = this->server.arg("novo_ip");
      this->saveServerIP(novoIP); 
      
      String html = "<html><head><meta charset='UTF-8'>";
      html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
      html += "<style>";
      html += "body { font-family: sans-serif; text-align: center; padding: 20px; color: #333; }";
      html += ".btn { display: inline-block; padding: 12px 24px; margin-top: 20px; background-color: #2196F3; color: white; text-decoration: none; border-radius: 5px; font-weight: bold; }";
      html += ".alerta { margin-top: 30px; padding: 15px; background-color: #fff3cd; color: #856404; border: 1px solid #ffeeba; border-radius: 5px; font-size: 0.9em; text-align: left; }";
      html += "</style></head><body>";
      
      html += "<h2 style='color: #4CAF50;'>IP do Servidor salvo com sucesso!</h2>";
      html += "<p>O novo IP configurado é: <br><strong style='font-size: 1.2em;'>" + novoIP + "</strong></p>";
      
      html += "<a href='/' class='btn'>Voltar à Página Inicial</a>";
      
      html += "<div class='alerta'>";
      html += "<h3 style='margin-top: 0;'>⚠️ Ação Necessária</h3>";
      html += "<p>Para que o novo IP entre em vigor, <strong>desligue e ligue " + this->id + "</strong>.</p>";
      html += "</div>";
      
      html += "</body></html>";
      
      this->server.send(200, "text/html", html);
    } else {
      this->server.send(400, "text/plain", "Erro: IP nao fornecido");
    }
  });

  server.begin();
  Serial.println("Servidor Web iniciado!");
}

void WifiController::handleWebServer() 
{
  server.handleClient();
}

void WifiController::loadServerIP() 
{
  EEPROM.begin(512);
  serverIP = "";
  for (int i = 0; i < 32; ++i) {
    char c = char(EEPROM.read(i));
    if (c == '\0' || c == 0xFF) break; 
    serverIP += c;
  }
}

void WifiController::saveServerIP(String newIP) {
  serverIP = newIP;
  for (unsigned int i = 0; i < newIP.length(); ++i) {
    EEPROM.write(i, newIP[i]);
  }
  EEPROM.write(newIP.length(), '\0');
  EEPROM.commit();
}