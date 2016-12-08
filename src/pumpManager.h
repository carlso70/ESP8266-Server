#pragma once
#include <Arduino.h>
#define PUMP_PIN D0

class PumpManager {
public:
  PumpManager();

  // Turns the pump on and off
  void activate(int on);

  ~PumpManager();
};
