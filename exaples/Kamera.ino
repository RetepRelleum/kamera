//----------------------------------------------------------------------------------------------------------------------
// This program is based on: WiFiClient from ESP libraries

#include <WiFi.h>
#include <Kamera.h>
#include <SSDP.h>
#include <Webserver.h>

WiFiClient client;
Kamera kamera("DIRECT-hKC1:DSC-HX50V", "ySocVxsG");
uint32_t timestamp = 0;
WiFiServer server(80);
Webserver webServer(&kamera, &server);

const char *assid = "DSC-HX50V";
const char *asecret = "Calanda3000";

void setup()
{
  Serial.begin(115200);
  delay(10);

  WiFi.mode(WIFI_MODE_APSTA);
  Serial.println("Creating Accesspoint");
  WiFi.softAP(assid, asecret, 1, 0, 5);
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());
  timestamp = millis();
  server.begin();

  printWifiStatus();
}
void loop()
{

  WiFi.localIP();

  kamera.loop();

  WiFi.softAPIP();

  if (WiFi.softAPgetStationNum() > 0)
  {
    webServer.loop();
  }
}

void printWifiStatus()
{

  // print the SSID of the network you're attached to:

  Serial.print("SSID: ");

  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:

  IPAddress ip = WiFi.localIP();

  Serial.print("IP Address: ");

  Serial.println(ip);

  // print the received signal strength:

  long rssi = WiFi.RSSI();

  Serial.print("signal strength (RSSI):");

  Serial.print(rssi);

  Serial.println(" dBm");
}