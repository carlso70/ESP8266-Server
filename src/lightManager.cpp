#include "LightManager.h"
#include "networkManager.h"

LightManager::LightManager() {
  pinMode(LED_PIN, OUTPUT);
}

void LightManager::activate(int on) {
  if (on == 1) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}

LightManager::~LightManager() {}
