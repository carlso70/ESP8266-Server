#pragma once

#include "ArduinoJson.h"
#include <Arduino.h>
#include "ESP8266WiFi.h"

struct requestValues {
  int ledCount;
  int brightness;
};

class NetworkManager {
private:
  class LightManager* lightManager;
public:
  // Constructors
  NetworkManager();
  ~NetworkManager();

  //Setters
  void setLightManager(LightManager* manager);

  void checkHttpServer();
  // Returns a requestValues struct that contains the Json data
  struct requestValues parseRequest(String request);
  void startConnection();
};
