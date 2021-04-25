#ifndef WEBCLIENT_H
#define WEBCLIENT_H

#include <WiFiClient.h>
#include <vector>
#include <SD.h>
#include <DateTime.h>

class WebClient
{
private:
    String mimeTypes[25][2] = {
        {"arc", "application/x-freearc"},
        {"bin", "application/octet-stream"},
        {"bmp", "image/bmp"},
        {"css", "text/css"},
        {"csv", "text/csv"},
        {"gif", "image/gif"},
        {"htm", "text/html"},
        {"html", "text/html"},
        {"ico", "image/x-icon"},
        {"jpeg", "image/jpeg"},
        {"jpg", "image/jpeg"},
        {"js", "application/javascript"},
        {"json", "application/json"},
        {"otf", "font/otf"},
        {"pdf", "application/pdf"},
        {"png", "image/png"},
        {"svg", "image/svg+xml"},
        {"ts", "application/typescript"},
        {"ttf", "font/ttf"},
        {"txt", "text/plain"},
        {"woff", "font/woff"},
        {"woff2", "font/woff2"},
        {"rec", "text/csv"},
        {"xml", "application/xml"},
        {"zip", "application/zip"}};
    int mineTypesSize = 25;
    String request;
    std::vector<WebClient *> *v;
    std::vector<WebClient *>::iterator ptr;
    void stop();
    String getMimeTypes(String request);
    String webRoot;
    int countRec;
    void printDirectory(File dir, int numTabs);
    File file;
    char buffer[256];
    String requestAdd;

public:
    WebClient(WiFiClient client, std::vector<WebClient *> *v,String webRoot);
    WiFiClient client;
    void begin();
    void loop();
};

#endif