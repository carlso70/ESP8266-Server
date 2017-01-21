#pragma once

#include <WiFiUdp.h>
#include "ArduinoJson.h"
#include <Arduino.h>
#include "ESP8266WiFi.h"

struct requestValues {
  bool lights;
  bool pump;
  String ssid;
  String key;
};

const char WiFiAPPSK[] = "sparkfun";

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
  bool startConnection(String SSID, String PASS);

  void setupAccessPoint();
  void checkAccessPoint();

  void sendSetupPackets();
  void checkUDPPackets();

  String scanWifi();
  // Returns a requestValues struct that contains the Json data
  struct requestValues parseRequest(String request);
};
