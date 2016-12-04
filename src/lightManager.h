#pragma once

#define LED_PIN D0

class LightManager {
public:

  // Constructors
  LightManager();
  ~LightManager();

  // Turn the lights on and off
  void activate(bool on);
};
