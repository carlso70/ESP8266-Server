#include <Arduino.h>
#include <String>
#include "FastLED.h"
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

  networkManager = new NetworkManager();
  networkManager->startConnection();
  lightManager = new LightManager();
  networkManager->setLightManager(lightManager);

}

void loop() {
  networkManager->checkHttpServer();
}
