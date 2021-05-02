#ifndef WEBSERVERXX_H
#define WEBSERVERXX_H

#include <Kamera.h>

class Webserver
{
private:
    Kamera *kamera;
    WiFiServer *server;
    String get;
    String url;
    byte favicon[318] = {0X00, 0X00, 0X01, 0X00, 0X01, 0X00, 0X10, 0X10, 0X10, 0X00, 0X01, 0X00, 0X04, 0X00, 0X28, 0X01, 0X00, 0X00, 0X16, 0X00,
                         0X00, 0X00, 0X28, 0X00, 0X00, 0X00, 0X10, 0X00, 0X00, 0X00, 0X20, 0X00, 0X00, 0X00, 0X01, 0X00, 0X04, 0X00, 0X00, 0X00,
                         0X00, 0X00, 0X80, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X10, 0X00, 0X00, 0X00, 0X00, 0X00,
                         0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0Xff, 0X84, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
                         0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
                         0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
                         0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
                         0X11, 0X10, 0X00, 0X00, 0X01, 0X10, 0X00, 0X00, 0X00, 0X10, 0X00, 0X00, 0X00, 0X10, 0X00, 0X00, 0X01, 0X01, 0X00, 0X00,
                         0X00, 0X11, 0X11, 0X00, 0X01, 0X01, 0X00, 0X00, 0X00, 0X10, 0X01, 0X01, 0X01, 0X01, 0X00, 0X01, 0X00, 0X10, 0X01, 0X01,
                         0X01, 0X01, 0X00, 0X10, 0X10, 0X10, 0X01, 0X01, 0X00, 0X00, 0X00, 0X10, 0X10, 0X10, 0X01, 0X01, 0X11, 0X00, 0X01, 0X01,
                         0X01, 0X00, 0X00, 0X01, 0X00, 0X10, 0X01, 0X01, 0X01, 0X00, 0X00, 0X01, 0X00, 0X10, 0X10, 0X01, 0X00, 0X10, 0X00, 0X01,
                         0X00, 0X10, 0X10, 0X01, 0X00, 0X10, 0X00, 0X01, 0X00, 0X10, 0X00, 0X00, 0X00, 0X00, 0X00, 0X01, 0X11, 0X00, 0X00, 0X00,
                         0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0Xff, 0Xff, 0X00, 0X00, 0Xff, 0Xf1,
                         0X00, 0X00, 0Xf9, 0Xfd, 0X00, 0X00, 0Xfd, 0Xfa, 0X00, 0X00, 0Xfc, 0X3a, 0X00, 0X00, 0Xfd, 0Xaa, 0X00, 0X00, 0Xed, 0Xaa,
                         0X00, 0X00, 0Xd5, 0Xaf, 0X00, 0X00, 0Xd5, 0Xa3, 0X00, 0X00, 0Xab, 0Xed, 0X00, 0X00, 0Xab, 0Xed, 0X00, 0X00, 0X6d, 0Xed,
                         0X00, 0X00, 0X6d, 0Xed, 0X00, 0X00, 0Xff, 0Xe3, 0X00, 0X00, 0Xff, 0Xff, 0X00, 0X00, 0Xff, 0Xff, 0X00, 0X00};

public:
    Webserver(Kamera *kamera, WiFiServer *server);
    String response(String in);

    void loop();
};

#endif