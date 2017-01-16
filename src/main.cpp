#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "pumpManager.h"
#include "networkManager.h"
#include "lightManager.h"

// Managers
NetworkManager* networkManager;
LightManager* lightManager;
PumpManager* pumpManager;

/*
#define LED_PIN D0
#define PUMP_PIN D2
*/

void setup() {
  Serial.begin(115200);

  pumpManager = new PumpManager();
  lightManager = new LightManager();
  networkManager = new NetworkManager(lightManager, pumpManager);

  networkManager->setupAccessPoint();

  /*
  networkManager->startConnection();
  */
}

void loop() {
  //  networkManager->checkHttpServer();
  if (WiFi.status() != WL_CONNECTED) {
    networkManager->checkAccessPoint();
  }else {
    networkManager->checkHttpServer();
    networkManager->checkUDPPackets();
  }
}
