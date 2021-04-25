//----------------------------------------------------------------------------------------------------------------------
// This program is based on: WiFiClient from ESP libraries


#include <WiFi.h>
#include <Kamera.h>
#include <SSDP.h>
#include <WiFiServer.h>


WiFiClient client;
Kamera kamera("DIRECT-hKC1:DSC-HX50V", "FvTLb6wv");
uint32_t timestamp = 0;
WiFiServer server(80);



void setup()
{
  Serial.begin(115200);
  delay(10);

  timestamp = millis();
}
void loop()
{

  kamera.loop();

  if (timestamp + 30000 < millis())
  {
    timestamp = millis();
    Serial.println();
    Serial.println(String("Total heap: ") + ESP.getHeapSize());
    Serial.println(String("Free heap: ") + ESP.getFreeHeap());
    Serial.println(String("Total PSRAM: ") + ESP.getPsramSize());
    Serial.println(String("Free PSRAM: ") + ESP.getFreePsram());
  }
}
