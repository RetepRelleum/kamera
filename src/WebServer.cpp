#include "WebServer.h"
using namespace ws;

void WebServerS::loop()
{
    webClient = new WebClient(server.available(), &v, webRoot);

    if (webClient->client)
    {
        webClient->begin();
        v.push_back(webClient);
    }
    else
    {
        delete webClient;
    }
    ptr = std::begin(v);
    for (int i = 0; i < v.size(); i++)
    {
        ptr[i]->loop();
    }
}

void WebServerS::begin(WiFiServer server, String webRoot)
{
    this->server = server;
    this->server.begin();
    if (webRoot.endsWith("/") | webRoot.endsWith("\\"))
    {
        webRoot = webRoot.substring(webRoot.length() - 1);
    }
    this->webRoot = webRoot;
}
WebServerS WebServer;