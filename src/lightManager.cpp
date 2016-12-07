#include "LightManager.h"
#include "networkManager.h"

LightManager::LightManager() {
  pinMode(LED_PIN, OUTPUT);
}

void LightManager::activate(bool on) {
  if (on) {
    Serial.println("Turning lights on");
    digitalWrite(LED_PIN, HIGH);
  } else {
    Serial.println("Turning lights off");
    digitalWrite(LED_PIN, LOW);
  }
}

LightManager::~LightManager() {}
