#include <Kamera.h>
#include <Arduino.h>
#define DEBUG 1
//#define DEBUG 0
Kamera::Kamera(String ssid, String password)
{
  this->ssid = ssid;
  this->password = password;
}

void Kamera::loop()
{
  switch (status)
  {
  case init:
    if (statusInit())
      status = ssdpS;
    break;
  case ssdpS:
    ssdp.loop();
    if (ssdp.ready())
    {
      status = initKamera;
    }
    break;
  case initKamera:
    Serial.println(getVersions());
    Serial.println(getAvailableApiList());
    Serial.println(startLiveview());
    status = liveViewS;
    timestampPic = millis();
    break;
  case liveViewS:
    if (timestampPic + 20000 > millis())
    {
      liveView();
    }
    else
    {
      Serial.println(stopLiveview());
      status = takePic;
    }
    break;
  case takePic:
    if (timestampPic + 1000 < millis())
    {
      timestampPic = millis();
      actTakePicture();
      if (count == 0)
      {
        Serial.println(stopLiveview());
      }
      count++;
      if (count > 3)
      {
        status = idle;
      }
    }
    break;
  case idle:

    break;
  }
}

int Kamera::statusInit()
{
  if (!WiFi.SSID().equals(ssid))
  {
    WiFi.disconnect();
  }
  int numSsid;
  numSsid = WiFi.scanNetworks();
  bool ok;
  ok = false;
  for (int thisNet = 0; thisNet < numSsid; thisNet++)
    if (WiFi.SSID(thisNet).equals(ssid))
      ok = true;
  if (!ok)
  {
    if (DEBUG)
    {
      Serial.print("WIFI Kamera not Found :");
      Serial.println(ssid);
    }
    return 0;
  }
  else
  {
    if (DEBUG)
    {
      Serial.print("WIFI Kamera Found :");
      Serial.println(ssid);
    }
  }
  Serial.println(ssid.c_str());
  Serial.println(password.c_str());
  WiFi.begin(ssid.c_str(), password.c_str());
  delay(2000);

  if (WiFi.status() != WL_CONNECTED)
  {
    if (DEBUG)
    {
      Serial.print("WIFI Kamera Login not OK :");
      Serial.println(ssid);
    }
    return 0;
  }
  if (DEBUG)
  {
    Serial.print("WIFI Kamera Login OK :");
    Serial.println(ssid);
  }
  return 1;
}

String Kamera::httpPost(String jString)
{
  if (DEBUG)
  {
    Serial.print("Msg send: ");
    Serial.println(jString);
    Serial.print("connecting to ");
    Serial.println(ssdp.getActionListHost());
  }

  if (!client.connect(ssdp.getActionListHost(), ssdp.getActionListPort()))
  {
    if (DEBUG)
    {
      Serial.println("connection failed");
      status = init;
    }
    return "";
  }
  else
  {
    if (DEBUG)
    {
      Serial.print("connected to ");
      Serial.print(ssdp.getActionListHost());
      Serial.print(":");
      Serial.println(ssdp.getActionListPort());
    }
  }
  String url = "/";
  // We now create a URI for the request
  if (!ssdp.getActionListPath().equals(""))
  {
    url += ssdp.getActionListPath();
    url += "/";
  }
  url += ssdp.getServiceType();

  if (DEBUG)
  {
    Serial.print("Requesting URL: ");
    Serial.println(url);
  }

  // This will send the request to the server
  client.print(String("POST " + url + " HTTP/1.1\r\n" + "Host: " + ssdp.getActionListHost().toString() + "\r\n"));
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(jString.length());
  // End of headers
  client.println();
  // Request body
  client.println(jString);
  if (DEBUG)
  {
    Serial.println("wait for data");
  }
  lastmillis = millis();
  while (!client.available() && millis() - lastmillis < 8000)
  {
  } // wait 8s max for answer

  // Read all the lines of the reply from server and print them to Serial
  String res;
  while (client.available())
  {
    String line = client.readStringUntil('\r');
    Serial.print(line);
    res = line.substring(line.indexOf("\"result\":") + 9, line.length() - 1);
  }
  if (DEBUG)
  {
    Serial.println();
    Serial.println("----closing connection----");
    Serial.println();
  }
  client.stop();
  return res;
}

String Kamera::getVersions()
{
  String son = json;
  son.replace("METHODE", "getVersions");
  String ret = httpPost(son);
  ret.replace("[[\"", "");
  ret.replace("\"]]", "");
  return ret;
}

String Kamera::getAvailableApiList()
{
  String son = json;
  son.replace("METHODE", "getAvailableApiList");
  String ret = httpPost(son);
  ret.replace("[[\"", "");
  ret.replace("\"]]", "");
  ret.replace("\",\"", "\r\n");
  return ret;
}

String Kamera::startLiveview()
{
  String son = json;
  son.replace("METHODE", "startLiveview");
  String ret = httpPost(son);
  ret.replace("[\"", "");
  ret.replace("\"]", "");

  liveViewUrl = ret;
  String sp = ret;

  return ret;
}

String Kamera::actTakePicture()
{
  String son = json;
  son.replace("METHODE", "actTakePicture");
  String ret = httpPost(son);
  return ret;
}

String Kamera::stopLiveview()
{
  String son = json;
  son.replace("METHODE", "stopLiveview");
  String ret = httpPost(son);
  return ret;
}
void Kamera::liveView()
{
  if (!liveViewInit)
  {
    String sp = liveViewUrl;
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
    IPAddress hostLv = IPAddress(ip[0], ip[1], ip[2], ip[3]);
    Serial.println(sp.substring(0, sp.indexOf("/")));
    long portLv = sp.substring(0, sp.indexOf("/")).toInt();
    String pathLv = sp.substring(sp.indexOf("/"), sp.length());

    if (DEBUG)
    {
      Serial.print("connecting to  liveView : ");
      Serial.print(hostLv.toString());
      Serial.print(":");
      Serial.println(portLv);
    }

    if (!clientLive.connect(hostLv, portLv))
    {

      return;
    }
    else
    {
      if (DEBUG)
      {
        Serial.print("connected to ");
        Serial.print(hostLv.toString());
        Serial.print(":");
        Serial.println(portLv);
      }
    }

    // This will send the request to the server
    clientLive.print(String("GET " + pathLv + " HTTP/1.1\r\n" + "Host: " + hostLv.toString() + "\r\n"));
    clientLive.println("Accept: image/gif, image/jpeg, */*");
    clientLive.println("Connection: close");
    // End of headers
    clientLive.println();
    // Request body

    if (DEBUG)
    {
      Serial.println("wait for data");
    }
    liveViewInit = true;
    while (!clientLive.available() && millis() - lastmillis < 8000)
    {
    } // wait 8s max for answer

    // Read all the lines of the reply from server and print them to Serial
    String res;
    if (clientLive.available())
    {
    }
    lastmillis = millis();
    while (!clientLive.available() && millis() - lastmillis < 8000)
    {
    } // wait 8s max for answer

    String data = "";
    uint8_t buffer[200];
    if (clientLive.available())
    {
      uint8_t ch = 9;
      while (clientLive.available())
      {
        ch = clientLive.read();
        Serial.write((char)ch);
        if (ch == 0XFF)
        {
          break;
        }
      }
      Serial.println();
      Serial.println("  --> Read Common Header ");
      int a = clientLive.read(buffer, 7);
      commonHeader = (CommonHeader *)buffer;
      Serial.print("Payload type :");
      Serial.println(commonHeader->payloadType, HEX);
      Serial.print("Sequence number:");
      Serial.println(swap_uint16(commonHeader->sequenceNumber));
      Serial.print("Time stamp:");
      Serial.println(swap_uint32(commonHeader->timeStamp));
      a = clientLive.read(buffer, 128);
      Serial.print("Start code  fixed (0x24, 0x35, 0x68, 0x79):");
      Serial.print(" 0x");
      Serial.print(buffer[0], HEX);
      Serial.print(" 0x");
      Serial.print(buffer[1], HEX);
      Serial.print(" 0x");
      Serial.print(buffer[2], HEX);
      Serial.print(" 0x");
      Serial.println(buffer[3], HEX);
      Serial.print("Payload data size:");
      Serial.println(buffer[4] << + buffer[5] * 256 + buffer[6]);
      Serial.print("Padding size:");
      Serial.println(buffer[7]);
      for (int i = 8; i < a; i++)
      {
        Serial.print(" 0x");
        Serial.println(buffer[i], HEX);
      }
    }
  }
  if (clientLive.available())
  {
    char buffer[200];
    int a = clientLive.read((uint8_t *)buffer, 200);
    for (int i = 0; i < a; i++)
    {
      Serial.print(" 0x");
      Serial.print(buffer[i], HEX);
    }
  }
  else
  {
    if (DEBUG)
    {
      Serial.println();
      Serial.println("----closing connection----");
      Serial.println();
    }
    clientLive.stop();
    status = idle;
  }
}
