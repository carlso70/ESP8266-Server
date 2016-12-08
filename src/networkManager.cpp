#include "networkManager.h"
#include "lightManager.h"
#include "pumpManager.h"
#include "wifiKeys.h"

WiFiServer server(80);

NetworkManager::NetworkManager(LightManager* lights, PumpManager* pumps) {

  lightManager = lights;
  pumpManager = pumps;

  WiFi.mode(WIFI_AP);
  // WiFi.begin(SSID, Network pass)
  WiFi.begin(SCOTTWIFI, SCOTTPASS);
  server.begin();
}

void NetworkManager::startConnection() {
  Serial.println("Connecting...");
  while(WiFi.status() != WL_CONNECTED) {
    delay(50);
  }
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void NetworkManager::checkHttpServer() {
  WiFiClient client = server.available();
  if (!client) {
    if (WiFi.status() != WL_CONNECTED) {
      startConnection();
    }
    return;
  }
  String request = client.readString();
  Serial.print("Recieved request: ");
  Serial.println(request);

  requestValues result = parseRequest(request);
  client.write("HTTP/1.0 200 OK\n\nFuck you buddy: ");
  client.write(String(result.lights).c_str());

  // apply leds
  lightManager->activate(result.lights);
  pumpManager->activate(result.pump);
}

/*
* Parse's the JSON request, and returns a requestValues struct
*/
struct requestValues NetworkManager::parseRequest(String request) {
  StaticJsonBuffer<200> jsonBuffer;

  // Skips header of Json
  char lastChar = 0;
  int bodyIndex = -1;
  for (int i = 3; i < request.length(); i++) {
     String before(request.substring(i - 5, i));
     if (before.endsWith("\r\n\r\n") || before.endsWith("\n\n")) {
       bodyIndex = i;
       break;
     }
  }

//TODO create exceptions for when there is no body found
  if (bodyIndex < 0) {
    Serial.println("NO BODY FOUND");
  }

  String requestStr(request.substring(bodyIndex));
  int jsonIndex = requestStr.indexOf("{");
  Serial.print("JSON ONLY Request: ");
  Serial.print(requestStr.substring(jsonIndex));

  // Extract properties from JSON
  JsonObject& root = jsonBuffer.parseObject(requestStr.substring(jsonIndex));
  int lights = root["Lights"].as<int>();
  int pump = root["Pump"].as<int>();

  requestValues a;
  a.lights = lights;
  a.pump = pump;

  return a;
}

NetworkManager::~NetworkManager() {}
