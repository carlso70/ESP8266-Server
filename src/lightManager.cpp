#include "LightManager.h"
#include "networkManager.h"

LightManager::LightManager() {
  pinMode(LED_PIN, OUTPUT);
}

void LightManager::activate(bool on) {
  if (on) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}

LightManager::~LightManager() {}
