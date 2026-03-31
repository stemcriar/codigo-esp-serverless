#include "WifiAP.h"

WifiAPController wifi;

String id = "CAR003"; 

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  Serial.println("{\"from\":\"ESP\",\"state\":\"OK\"}");
  Serial.println("{\"ESP\":\"Hello\"}");
  
  wifi.startAP(id.c_str(), "");
}

void loop() {
  wifi.handleServers();
}