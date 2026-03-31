#include "WifiAP.h"

WifiAPController wifi;

const String id = "CAR003";

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  wifi.loadID(id);
  
  Serial.println("{\"from\":\"ESP\",\"state\":\"OK\"}");
  Serial.println("{\"ESP\":\"Hello\"}");

  wifi.startAP();
}

void loop() {
  wifi.handleServers();
}