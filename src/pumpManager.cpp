#include "pumpManager.h"

PumpManager::PumpManager() {
  pinMode(PUMP_PIN, OUTPUT);
}

void PumpManager::activate(bool on) {
  if (on) {
    Serial.println("Turning pump on");
    digitalWrite(PUMP_PIN, HIGH);
  } else {
    Serial.println("Turning pump off");
    digitalWrite(PUMP_PIN, LOW);
  }
}

PumpManager::~PumpManager() {}
