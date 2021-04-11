#ifndef SSDP_H
#define SSDP_H
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#define DEBUG 1
//#define DEBUG 0

class SSDP
{
private:
    enum Status
    {
        init,
        searchSend,
        searchRead,
        getLocationInfo,
        readLocationInfo,
        idle
    };
    Status status = init;
    uint32_t timestamp = millis();
    WiFiUDP udp;
    WiFiClient client;
    String location = "";
    IPAddress host;
    int port = 0;
    String xml = "";

    void readXml();
    void pLocation(String s);
    int statusInit();
    int statusSearchSend();
    int statusSearchRead();
    int statusGetLocationInfo();
    int statusReadLocationInfo();
    String cameraName;
    String serviceType;
    String actionList_URL;

public:
    void loop();
    bool ready();
   String getCameraName();
   String getServiceType();
   IPAddress getActionListHost();
   int getActionListPort();
   String getActionListPath();
};

#endif