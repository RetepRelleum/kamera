#ifndef LIVEVIEW_H
#define LIVEVIEW_H

#include <WiFi.h>
#include <JPEGDEC.h>

struct CommonHeader
{
    uint8_t payloadType;
    uint16_t sequenceNumber;
    uint32_t timeStamp;
};

class LiveView
{
private:
    String liveViewUrl;
    IPAddress host;
    long port;
    bool isConnect;
    String path;
    WiFiClient client;
    uint32_t lastmillis;
    uint8_t buffer[200];
    CommonHeader *commonHeaderP;
    CommonHeader commonHeader;
    uint32_t payloadDataSize;
    uint8_t paddingSize;
    uint8_t *jPegBuffer;
    JPEGDEC jpeg;
    bool isMotion = false;
    bool readCommonHeader();
    bool readPayloadHeader();
    bool waitForData(int size);
    bool readJpegData();
    bool decodeJpeg();

    uint16_t swap_uint16(uint16_t val)
    {
        return (val << 8) | (val >> 8);
    }
    uint32_t swap_uint32(uint32_t val)
    {
        val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
        return (val << 16) | (val >> 16);
    }
    void init(String liveViewUrl);

public:
    bool connect(String liveViewUrl);
    bool loop();
    bool motionDetect();
    void disconnect();
    uint8_t *getJpeg();
    uint32_t getJpegSize();
};

#endif