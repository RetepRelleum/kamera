#ifndef Kamera_H
#define Kamera_H
#include <WiFi.h>
#include <WiFiUdp.h>
#include <JPEGDEC.h>
#include <SSDP.h>
#include <LiveView.h>


struct CommonHeader
{
    uint8_t payloadType;
    uint16_t sequenceNumber;
    uint32_t timeStamp;
};


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

    CommonHeader *commonHeader;
    int count = 0;
    Status status = init;
    char buffer[200];
    SSDP ssdp;
    uint8_t *lvBuffer;
    uint32_t lastmillis;
    String ssid;
    String password;
    WiFiClient client;
    WiFiClient clientLive;
    String json = "{\"version\":\"1.0\",\"id\":1,\"method\":\"METHODE\",\"params\":[]}";
    String liveViewUrl;
    String httpPost(String jString);

    uint32_t timestampPic = millis();

    uint16_t swap_uint16(uint16_t val)
    {
        return (val << 8) | (val >> 8);
    }
    uint32_t swap_uint32(uint32_t val)
    {
        val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
        return (val << 16) | (val >> 16);
    }


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