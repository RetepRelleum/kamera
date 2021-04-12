#ifndef Kamera_H
#define Kamera_H
#include <WiFi.h>
#include <WiFiUdp.h>

#include <SSDP.h>
#define DEBUG 1
//#define DEBUG 0

class Kamera
{
private:
    enum Status
    {
        init,
        ssdpS,
        initKamera,
        takePic,
        idle
    };
    int count=0;
    Status status = init;
    SSDP ssdp;
    uint32_t lastmillis;
    String ssid;
    String password;
    WiFiClient client;
    String json = "{\"version\":\"1.0\",\"id\":1,\"method\":\"METHODE\",\"params\":[]}";
    String httpPost(String jString);
    uint32_t timestampPic=millis();


public:
    Kamera(String ssid, String password);
    void loop();
    int statusInit();
    String getVersions();
    String getAvailableApiList();
    String startLiveview();
    String stopLiveview();
    String actTakePicture();
};

#endif