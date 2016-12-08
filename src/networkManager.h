#pragma once

#include "ArduinoJson.h"
#include <Arduino.h>
#include "ESP8266WiFi.h"

struct requestValues {
  int lights = 0;
  int pump = 0;
};

class NetworkManager {
private:
  class LightManager* lightManager;
  class PumpManager* pumpManager;
public:
  // Constructors
  NetworkManager(LightManager* lights, PumpManager* pumps);
  ~NetworkManager();

  //Setters
  void setLightManager(LightManager* manager);
  void setPumpManager(PumpManager* manager);

  void checkHttpServer();
  // Returns a requestValues struct that contains the Json data
  struct requestValues parseRequest(String request);
  void startConnection();
};
