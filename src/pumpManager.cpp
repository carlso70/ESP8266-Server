#include "pumpManager.h"

PumpManager::PumpManager() {
  pinMode(PUMP_PIN, OUTPUT);
}

void PumpManager::activate(bool on) {
  if (on) {
    digitalWrite(PUMP_PIN, HIGH);
  } else {
    digitalWrite(PUMP_PIN, LOW);
  }
}

PumpManager::~PumpManager() {}
