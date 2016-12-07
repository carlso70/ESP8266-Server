#include "networkManager.h"
#include "lightManager.h"
#include "pumpManager.h"
#include "wifiKeys.h"

WiFiServer server(80);

NetworkManager::NetworkManager(LightManager* lights, PumpManager* pumps) {
  lightManager = lights;
  pumpManager = pumps;
}

void NetworkManager::startConnection(String SSID, String PASS) {
  // Will not return until wifi connetion succeeds or fails
  int status = WiFi.begin(SSID.c_str(), PASS.c_str());

  bool isConnected = status == WL_CONNECTED;

  if(!isConnected) {
    Serial.println("Connection failed");
    return;
  }

  Serial.println("Connecting...");

  // disable the access point
  WiFi.softAPdisconnect(true);

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void NetworkManager::checkHttpServer() {
  WiFiClient client = server.available();
  if (!client) {
    if (WiFi.status() != WL_CONNECTED) {
      //TODO reset and go back access point
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
  if (lightManager != 0) {
    lightManager->activate(result.lights);
  }
  if (pumpManager != 0) {
    pumpManager->activate(result.pump);
  }
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
  bool lights = root["Lights"].as<bool>();
  bool pump = root["Pump"].as<bool>();
  String ssid = root["SSID"].asString();
  String key = root["Key"].asString();

  requestValues a;

  a.lights = lights;
  a.pump = pump;
  a.ssid = ssid;
  a.key = key;

  return a;
}

void NetworkManager::setupAccessPoint()
{
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
  String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "ESP8266 Hydro Thing " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
  AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);

  server.begin();
}

void NetworkManager::checkAccessPoint() {
  WiFiClient client = server.available();
  if (!client) {
    delay(50);
    return;
  }

  String request = client.readString();
  requestValues result = parseRequest(request);

  // Attempt to start connection with values
  if (result.ssid != NULL) {
    startConnection(result.ssid, result.key);
  }

  client.flush();

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  s += scanWifi();
  s += "Fuck you buddy";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}

String NetworkManager::scanWifi() {
  int n = WiFi.scanNetworks();
  if (n == 0) {
    return "No networks found";
  }

  String s = "Found ";
  s += String(n);
  s += " networks <br>";
  for (int i = 0; i < n; i++) {

    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(WiFi.SSID(i));
    Serial.print("\n");

    s += String(i + 1);
    s += ": ";
    s += WiFi.SSID(i);
    s += "<br>";
  }
  return s;
}

NetworkManager::~NetworkManager() {}
