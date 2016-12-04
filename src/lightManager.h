#pragma once
#include "FastLED.h"

#define BRIGHTNESS 255
#define NUM_LEDS 300
#define DATA_PIN 3

class LightManager {
private:
  // Number of Leds lit
  int numLit;
  CRGB leds[NUM_LEDS];

public:

  // Constructors
  LightManager();
  ~LightManager();

  // Applies the selected effect, and lights the leds
  void applyLeds(struct requestValues request);

  // Led settings
  void cylon();
  void fadeall();
  void applyColor(int red, int green, int blue, int count);

};
