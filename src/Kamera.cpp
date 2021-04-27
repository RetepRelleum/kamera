#include <Kamera.h>
#include <Arduino.h>

#define DEBUG 0
//#define DEBUG 0

Kamera::Kamera(String ssid, String password)
{
  this->ssid = ssid;
  this->password = password;
}

void Kamera::loop()
{

  String deb;
  String deb2;
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
    deb = getVersions();
    deb2 = getAvailableApiList();
    if (DEBUG)
    {
      Serial.println(deb);
      Serial.println(deb2);
    }
    status = liveViewS;
    deb = startLiveview();
    if (DEBUG)
    {
      Serial.println(deb);
    }

    break;
  case liveViewS:
    if (!liveView.loop())
    {
      status = liveViewS;
    }
    break;
  case takePic:
    deb = actTakePicture();
    if (DEBUG)
    {
      Serial.println(deb);
    }
    deb = startLiveview();
    status = liveViewS;
    break;
  case idle:
    break;
  }
  if (liveView.motionDetect())
  {
    deb = stopLiveview();
    if (DEBUG)
    {
      Serial.println("Motion detect");
      Serial.println(deb);
    }

    status = takePic;
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
    if (DEBUG)
    {
      Serial.print(line);
    }
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
  liveView.connect(liveViewUrl);
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
  liveView.disconnect();
  String son = json;
  son.replace("METHODE", "stopLiveview");
  String ret = httpPost(son);
  return ret;
}

uint8_t *Kamera::getJpeg()
{
  return liveView.getJpeg();
}

    uint32_t Kamera::getJpegSize(){
      return liveView.getJpegSize();
    }