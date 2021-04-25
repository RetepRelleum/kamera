#ifndef Kamera_H
#define Kamera_H
#include <WiFi.h>
#include <WiFiUdp.h>
#include <JPEGDEC.h>
#include <SSDP.h>
#include <LiveView.h>

class Kamera
{
private:
    enum Status
    {
        init,
        ssdpS,
        initKamera,
        takePic,
        liveViewS,
        analyseMotion,
        idle
    };
    LiveView liveView;
    SSDP ssdp;
    int count = 0;
    Status status = init;
    uint32_t lastmillis;
    String ssid;
    String password;
    WiFiClient client;
    WiFiClient clientLive;
    String json = "{\"version\":\"1.0\",\"id\":1,\"method\":\"METHODE\",\"params\":[]}";
    String liveViewUrl;
    String httpPost(String jString);
    uint32_t timestampPic = millis();

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