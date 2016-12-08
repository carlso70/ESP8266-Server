#pragma once

#define LED_PIN D2

class LightManager {
public:

  // Constructors
  LightManager();
  ~LightManager();

  // Turn the lights on and off
  void activate(int on);
};
