#include "networkManager.h"
#include "lightManager.h"
#include "pumpManager.h"

WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char replyPacekt[] = "Hi there! Got the message :-)";  // a reply string to send back

WiFiServer server(80);

NetworkManager::NetworkManager(LightManager* lights, PumpManager* pumps) {
  lightManager = lights;
  pumpManager = pumps;
}

/* Attempts to connect to local WiFi with name SSID, and pass PASS.
* Returns false if failed
*/
bool NetworkManager::startConnection(String SSID, String PASS) {
  // Will not return until wifi connetion succeeds or fails
  Serial.print("SSID = ");
  Serial.print(SSID);
  Serial.print(" Pass = ");
  Serial.print(PASS);

  int status = WiFi.begin(SSID.c_str(), PASS.c_str());

  bool isConnected = status == WL_CONNECTED;

  delay(10000);
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Connection failed");

    return false;
  }

  Serial.println("Connecting...");

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

  return true;
}


/*
*  Checks the HTTP Server that is connected to the WiFi. Awaits JSON Responses
*  Does not check when in Access Point Mode
*/
void NetworkManager::checkHttpServer() {
  WiFiClient client = server.available();
  if (!client) {
    if (WiFi.status() != WL_CONNECTED) {
      //TODO reset and go back access point

    }
    return;
  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

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

  bool lights = root["Lights"].as<bool>();
  bool pump = root["Pump"].as<bool>();
  String ssid = root["SSID"].asString();
  String key = root["Key"].asString()

  requestValues a;

  a.lights = lights;
  a.pump = pump;
  a.ssid = ssid;
  a.key = key;

  return a;
}

/*
* Sets up device to function as a local Access Point. Awaits for requests to
* Connect to user's Wifi
*/
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
  String AP_NameString = "HydroGrow";

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
  AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);

  server.begin();
}

/*
*  Checks the Access Point for JSON requests with SSID and PASS to
*  local Network
*/
void NetworkManager::checkAccessPoint() {
  if (WiFi.getMode() != WIFI_AP) {
    setupAccessPoint();
    delay(50);
  }

  Serial.println("Checking ap \n\n");

  WiFiClient client = server.available();
  if (!client) {
    delay(50);
    return;
  }

  String request = client.readString();
  client.flush();
  requestValues result = parseRequest(request);
  bool didSucceed = false;

  // Attempt to start connection with values
  if (result.ssid != NULL) {
    didSucceed = startConnection(result.ssid, result.key);
  }

  WiFi.softAPdisconnect(didSucceed);
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

// send out udp packets telling that you are in access point mode, and need to be
// connected to wifi
void NetworkManager::sendSetupPackets() {
  Udp.beginPacket(Udp.remoteIP(), localUdpPort);
  Udp.write("In access point mode");
  Udp.endPacket();
  delay(100);
}

void NetworkManager::checkUDPPackets(){
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);

    // send back a reply, to the IP address and port we got the packet from
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    if (incomingPacket[0] = 'I'){
      Udp.write(WiFi.localIP().toString().c_str());
      Serial.println("Recieved Packet");
      Udp.endPacket();
    }
  }
}

NetworkManager::~NetworkManager() {}
