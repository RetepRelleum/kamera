#ifndef WEBSERVER_H
#define WEBSERVER_H
#include <Arduino.h>
#include <WiFiServer.h>
#include <WiFiClient.h>
#include <vector>
#include "WebClient.h"
namespace ws
{
    class WebServerS
    {
    private:
        WiFiServer server;
        std::vector<WebClient *> v;
        std::vector<WebClient *>::iterator ptr;
        String webRoot;
        WebClient *webClient;
    public:
        void begin(WiFiServer server, String webRoot = "/webRoot");
        void loop();
    };
} // namespace ws
extern ws::WebServerS WebServer;
using namespace ws;

#endif
