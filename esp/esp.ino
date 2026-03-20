#include "Wifi.h"
#include "webSocketClient.h"
#include "serial_comm.h"

WifiManager wifi;
Serial_comm arduino;

const String ip = "12.1.15.83"; 

const int port = 1801;

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("\nStarting Esp");

  arduino.doHandshake("ESP", "OK", "ARD");
  Serial.print("The type is: "); Serial.println(arduino.getReceivedType());
  setTypeToWs(arduino.getReceivedType());
  
  wifi.startWiFi();
  arduino.sendJson("ESP_IP", wifi.ip);

  startWebSocketClient(ip, port);
  arduino.sendJson("ws_client", "started");
  updateWebsocketClient();
  Serial.println("End of Setup");
};

void loop() {
  updateWebsocketClient();
  arduino.getJson();
  if (arduino.jsonUpdateCheck()){
    arduino.serializeCurrentJson();
    sendMessageWsClient(arduino.serializedCurrentJson);
  };
};
