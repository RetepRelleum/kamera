#include <SSDP.h>

static const IPAddress SSDP_MULTICAST_ADDR(239, 255, 255, 250);
#define SSDP_PORT 1900

void SSDP::loop()
{
    switch (status)
    {
    case init:
        if (statusInit())
            status = searchSend;
        else
            status = init;
        timestamp = millis();
        break;
    case searchSend:
        if (statusSearchSend())
            status = searchRead;
        else
            status = init;
        timestamp = millis();
        break;
    case searchRead:
        statusSearchRead();
        timestamp = millis();
        break;
    case getLocationInfo:
        if (statusGetLocationInfo())
            status = readLocationInfo;
        else
            status = init;
        timestamp = millis();
        break;
    case readLocationInfo:
        statusReadLocationInfo();
        timestamp = millis();
        break;
    }
    if ((millis() > timestamp + 120000))
    {
        if (DEBUG)
        {
            Serial.println("Status to init");
        }
        status = init;
        timestamp = millis();
    }
}
int SSDP::statusInit()
{
    udp.flush();
    udp.stop();
    int res = udp.beginMulticast(IPAddress(SSDP_MULTICAST_ADDR), SSDP_PORT);
    if (DEBUG)
    {
        Serial.println("SSDP::statusInit");
        Serial.print("  beginMulticast :");
        Serial.println(res);
    }
    return res;
}

int SSDP::statusSearchSend()
{
    int res;
    res = udp.beginMulticastPacket();
    if (!res)
        return res;
    if (DEBUG)
    {
        Serial.println("SSDP::statusSearchSend");
        Serial.print("  beginMulticastPacket :");
        Serial.println(res);
    }
    String a = "M-SEARCH * HTTP/1.1\r\n";
    udp.write((const uint8_t *)a.c_str(), a.length());
    if (DEBUG)
    {
        Serial.write("  ->");
        Serial.write((const uint8_t *)a.c_str(), a.length());
    }
    a = "HOST: 239.255.255.250:1900\r\n";
    udp.write((const uint8_t *)a.c_str(), a.length());
    if (DEBUG)
    {
        Serial.write("  ->");
        Serial.write((const uint8_t *)a.c_str(), a.length());
    }
    a = "MAN: \"ssdp:discover\"\r\n";
    udp.write((const uint8_t *)a.c_str(), a.length());
    if (DEBUG)
    {
        Serial.write("  ->");
        Serial.write((const uint8_t *)a.c_str(), a.length());
    }
    a = "MX: 2\r\n";
    udp.write((const uint8_t *)a.c_str(), a.length());
    if (DEBUG)
    {
        Serial.write("  ->");
        Serial.write((const uint8_t *)a.c_str(), a.length());
    }
    a = "ST: urn:schemas-sony-com:service:ScalarWebAPI:1\r\n";
    udp.write((const uint8_t *)a.c_str(), a.length());
    if (DEBUG)
    {
        Serial.write("  ->");
        Serial.write((const uint8_t *)a.c_str(), a.length());
    }
    a = "USER-AGENT: esp32/01 UPnP/1.0 kamera/0.1 \r\n";
    udp.write((const uint8_t *)a.c_str(), a.length());
    udp.print("\r\n");
    res = udp.endPacket();
    if (DEBUG)
    {
        Serial.write("  ->");
        Serial.write((const uint8_t *)a.c_str(), a.length());
        Serial.print("\r\n");
        Serial.print("  endPacket :");
        Serial.println(res);
    }
    return res;
}
int SSDP::statusSearchRead()
{
    udp.parsePacket();
    while (udp.available())
    {
        String s;
        s = udp.readStringUntil('\n');
        s.replace("\r", "");
        pLocation(s);
        if (DEBUG)
        {
            Serial.print("  <-");
            Serial.println(s);
        }
    }
    return 1;
}
void SSDP::pLocation(String s)
{
    if (s.startsWith("LOCATION:"))
    {
        String sp = s;
        sp.replace("LOCATION: ", "");
        location = sp;
        sp.replace("http://", "");
        int ip[4];
        ip[0] = sp.substring(0, sp.indexOf(".")).toInt();
        sp.remove(0, sp.indexOf(".") + 1);
        ip[1] = sp.substring(0, sp.indexOf(".")).toInt();
        sp.remove(0, sp.indexOf(".") + 1);
        ip[2] = sp.substring(0, sp.indexOf(".")).toInt();
        sp.remove(0, sp.indexOf(".") + 1);
        ip[3] = sp.substring(0, sp.indexOf(":")).toInt();
        sp.remove(0, sp.indexOf(":") + 1);
        host = IPAddress(ip[0], ip[1], ip[2], ip[3]);
        port = sp.substring(0, sp.indexOf("/")).toInt();
        xml = sp.substring(sp.indexOf("/") + 1, sp.length());
        status = getLocationInfo;
    }
}
int SSDP::statusGetLocationInfo()
{

    int res = client.connect(host, port);
    if (!res)
        return res;
    String a = "GET /";
    a += xml;
    a += " HTTP/1.1";
    client.println(a);
    if (DEBUG)
    {
        Serial.print("  ->");
        Serial.println(a);
    }
    a = "Host: ";
    a += host.toString();
    client.println(a);
    if (DEBUG)
    {
        Serial.print("  ->");
        Serial.println(a);
    }
    a = "Connection: close";
    client.println(a);
    if (DEBUG)
    {
        Serial.print("  ->");
        Serial.println(a);
    }
    a = "";
    client.println(a);
    if (DEBUG)
    {
        Serial.print("  ->");
        Serial.println(a);
    }
    return res;
}
int SSDP::statusReadLocationInfo()
{
    while (client.available())
    {
        String s = client.readStringUntil('>');
        if (DEBUG)
        {
            Serial.print("  <-");
            Serial.print(s);
            Serial.println(">");
        }
        if (s.indexOf("<friendlyName") > -1)
        {
            cameraName = client.readStringUntil('<');
            Serial.print(cameraName);
            Serial.print("<");
        }
        if (s.indexOf("<av:X_ScalarWebAPI_ServiceType") > -1)
        {
            serviceType = client.readStringUntil('<');
            Serial.print(serviceType);
            Serial.print("<");
        }
        if (s.indexOf("<av:X_ScalarWebAPI_ActionList_URL") > -1)
        {
            actionList_URL = client.readStringUntil('<');
            Serial.print(actionList_URL);
            Serial.print("<");
        }
    }

    if (!client.connected())
    {
        status = idle;
        Serial.println();
        Serial.println("disconnecting from server.");
        client.stop();
        if (DEBUG)
        {
            Serial.print("CameraName : ");
            Serial.println(cameraName);
            Serial.print("ServiceType : ");
            Serial.println(serviceType);
            Serial.print("ActionList_URL : ");
            Serial.println(actionList_URL);
        }
    }
    return 1;
}
bool SSDP::ready()
{
    return status == idle;
}
String SSDP::getCameraName()
{
    return cameraName;
}
String SSDP::getServiceType()
{
    return serviceType;
}
IPAddress SSDP::getActionListHost()
{
    String sp = actionList_URL;
    sp.replace("http://", "");
    int ip[4];
    ip[0] = sp.substring(0, sp.indexOf(".")).toInt();
    sp.remove(0, sp.indexOf(".") + 1);
    ip[1] = sp.substring(0, sp.indexOf(".")).toInt();
    sp.remove(0, sp.indexOf(".") + 1);
    ip[2] = sp.substring(0, sp.indexOf(".")).toInt();
    sp.remove(0, sp.indexOf(".") + 1);
    ip[3] = sp.substring(0, sp.indexOf(":")).toInt();
    return IPAddress(ip[0], ip[1], ip[2], ip[3]);
}
int SSDP::getActionListPort()
{
    String sp = actionList_URL;
    sp.replace("http://", "");
    return sp.substring(sp.indexOf(":")+1, sp.indexOf("/")).toInt();
};
String SSDP::getActionListPath()
{
    String sp = actionList_URL;
    sp.replace("http://", "");
    if (cameraName.indexOf("DSC-HX50")>-1)
    return "";
    return sp.substring(sp.indexOf("/"));
};