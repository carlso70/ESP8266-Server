#include "LightManager.h"
#include "networkManager.h"

LightManager::LightManager() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000);
  FastLED.setBrightness(BRIGHTNESS);

}

void LightManager::applyLeds(struct requestValues request) {
  for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = i < request.ledCount ? CRGB::White : CRGB::Black;
  }

  FastLED.setBrightness(request.brightness);
  FastLED.show();
}

void LightManager::applyColor(int red, int green, int blue, int count) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = i < count ? CRGB(red,green,blue) : CRGB::Black;
  }
}

LightManager::~LightManager() {}
