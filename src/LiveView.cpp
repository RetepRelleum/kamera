#include <LiveView.h>>
#include <Arduino.h>

#define DEBUG 1
//#define DEBUG 0

double xrgb[9][3];
size_t sw, sh;
double sumE;

int drawMCU(JPEGDRAW *pDraw)
{

  size_t x = 0;
  size_t k = 0;
  if (DEBUG)
  {
    Serial.print("pDraw->iBpp :");
    Serial.print(pDraw->iBpp);
    Serial.print(" pDraw->iHeight :");
    Serial.print(pDraw->iHeight);
    Serial.print(" pDraw->iWidth :");
    Serial.print(pDraw->iWidth);
    Serial.print(" pDraw->pPixels :");
    Serial.print(pDraw->pPixels[0]);
    Serial.print(" pDraw->x :");
    Serial.print(pDraw->x);
    Serial.print(" pDraw->y :");
    Serial.println(pDraw->y);
  }
  for (size_t w = 0; w < pDraw->iWidth; w++)
  {
    for (size_t h = 0; h < pDraw->iHeight; h++)
    {
      if (DEBUG)
      {
        Serial.print(pDraw->x + w);
        Serial.print(" ");
        Serial.print(pDraw->y + h);
        Serial.print(" ");
        Serial.print(pDraw->pPixels[x], HEX);
      }
      k = (pDraw->x + w) / (sw / 3) + 3 * (pDraw->y + h) / (sh / 3);
      x = w * pDraw->iHeight + h;
      if (DEBUG)
      {
        Serial.print(" ");
        Serial.print(k);
        Serial.print(" ");
        Serial.println(x);
      }
      xrgb[k][0] += (uint8_t)((pDraw->pPixels[x] & 0xF800) >> 11);
      xrgb[k][1] += (uint8_t)((pDraw->pPixels[x] & 0x07E0) >> 5);
      xrgb[k][2] += (uint8_t)(pDraw->pPixels[x] & 0x001F);
    }
  }

  return 1;
}


void LiveView::init(String liveViewUrl)
{
    this->liveViewUrl = liveViewUrl;
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
    host = IPAddress(ip[0], ip[1], ip[2], ip[3]);
    long port = sp.substring(0, sp.indexOf("/")).toInt();
    path = sp.substring(sp.indexOf("/"), sp.length());
}



bool LiveView::connect(String liveViewUrl)
{
    init(liveViewUrl);
    if (DEBUG)
    {
        Serial.print("connecting to  liveView : ");
        Serial.print(host.toString());
        Serial.print(":");
        Serial.println(port);
    }

    if (!client.connect(host, port))
    {
        return false;
    }
    else
    {
        if (DEBUG)
        {
            Serial.print("connected to ");
            Serial.print(host.toString());
            Serial.print(":");
            Serial.println(port);
        }
        // This will send the request to the server
        client.print(String("GET " + path + " HTTP/1.1\r\n" + "Host: " + host.toString() + "\r\n"));
        client.println("Accept: image/gif, image/jpeg, */*");
        client.println("Connection: close");
        // End of headers
        client.println();
        // Request body
        return true;
    }
}

void LiveView::loop()
{
    if (!client.connected()) 
    {
        connect(liveViewUrl);
    }
    else
    {
        if (DEBUG)
        {
            Serial.println("wait for data");
        }
        if (!waitForData(8))
        {
            client.stop();
            return;
        }

        if (!readCommonHeader())
        {
            client.stop();
            return;
        }

        if (!waitForData(128 + 8))
        {
            client.stop();
            return;
        }
        if (!readPayloadHeader())
        {
            client.stop();
            return;
        }
        if (commonHeader.payloadType == 0X01)
        {
            if (!readJpegData())
            {
                client.stop();
                return;
            };
        }
        else
        {
        }
    }
}

bool LiveView::readCommonHeader()
{
    String s = client.readStringUntil(0XFF);
    if (DEBUG)
    {
        Serial.println();
        Serial.println("  --> Read Common Header ");
    }
    int a = client.read(buffer, 7);
    commonHeaderP = (CommonHeader *)buffer;
    commonHeader.payloadType = commonHeaderP->payloadType;
    commonHeader.sequenceNumber = swap_uint16(commonHeaderP->sequenceNumber);
    commonHeader.timeStamp = swap_uint32(commonHeaderP->timeStamp);
    if (DEBUG)
    {
        Serial.print("Payload type :");
        Serial.println(commonHeaderP->payloadType, HEX);
        commonHeader.payloadType = commonHeaderP->payloadType;

        Serial.print("Sequence number:");
        Serial.println(swap_uint16(commonHeaderP->sequenceNumber));
        commonHeader.sequenceNumber = swap_uint16(commonHeaderP->sequenceNumber);
        Serial.print("Time stamp:");
        Serial.println(swap_uint32(commonHeaderP->timeStamp));
    }
}

bool LiveView::readPayloadHeader()
{
    int a = client.read(buffer, 128 + 8);
    if (DEBUG)
    {
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
    }
    if (!(buffer[0] == 0x24 && buffer[1] == 0x35 && buffer[2] == 0x68 && buffer[3] == 0x79))
    {
        return false;
    }
    payloadDataSize = buffer[4] * pow(2, 16) + buffer[5] * pow(2, 8) + buffer[6];
    paddingSize = buffer[7];
    if (DEBUG)
    {
        Serial.println(payloadDataSize);
        Serial.print("Padding size:");
        Serial.println(buffer[7]);
        Serial.print("Rest :");

        for (int i = 8; i < a; i++)
        {
            Serial.print(" 0x");
            Serial.print(buffer[i], HEX);
        }
        Serial.println();
    }
    return true;
}

bool LiveView::waitForData(int size)
{
    while ((client.available() <= size) && millis() - lastmillis < 8000)
    {
    } // wait 8s max for answer
    lastmillis = millis();
    if (!client.available())
    {
        if (DEBUG)
        {
            Serial.println("No Date");
        }
        return false;
    }
    return true;
}

bool LiveView::readJpegData()
{
    free(jPegBuffer);
    jPegBuffer = (uint8_t *)malloc(payloadDataSize + paddingSize);
    uint32_t c = 0;
    uint32_t ix = 0;
    while (ix <= payloadDataSize + paddingSize)
    {
        if (((payloadDataSize + paddingSize) - ix) >= 200)
        {
            if (!waitForData(200))
                return false;
            c = client.read(buffer, 200);
        }
        else
        {
            if (!waitForData(((payloadDataSize + paddingSize) - ix)))
                return false;
            c = client.read(buffer, ((payloadDataSize + paddingSize) - ix));
        }
    }
    for (size_t i = 0; i < c; i++)
    {
        jPegBuffer[ix] = buffer[i];
    }
    return true;
}

bool LiveView::decodeJpeg()
{
    {
        if (jpeg.openRAM((uint8_t *)jPegBuffer,    payloadDataSize , drawMCU))
        {
            if (DEBUG)
            {
                Serial.println("Successfully opened JPEG image");
                Serial.printf("Image size: %d x %d, orientation: %d, bpp: %d\n", jpeg.getWidth(), jpeg.getHeight(), jpeg.getOrientation(), jpeg.getBpp());
            }
        }
        else
        {
            Serial.println("Not Successfully opened JPEG image:");
            Serial.println(jpeg.getLastError());
        }
        for (size_t i = 0; i < 9; i++)
        {
            for (size_t i2 = 0; i2 < 3; i2++)
            {
                xrgb[i][i2] = 0;
            }
        }
        sw = jpeg.getWidth();
        sh = jpeg.getHeight();
        jpeg.decode(0, 0, 0);
        jpeg.close();
    }
    double sum = 0;
    for (size_t i = 0; i < 9; i++)
    {
        if (i % 3 == 0)
        {
            Serial.println();
        }
        for (size_t i2 = 0; i2 < 3; i2++)
        {
            Serial.print(xrgb[i][i2] / (sw * sh / 9));
            sum += xrgb[i][i2] / (sw * sh / 9);
            Serial.print(" ");
        }
        Serial.print(" ¦ ");
    }
    Serial.println();
    Serial.println(sum);
    Serial.println();

    if (!isnan(sum))
    {
        Serial.println(abs(100.0 / sumE * (sum - sumE)));
        sumE = sum;
    }

    if (DEBUG)
    {
        Serial.println();
        Serial.println("----closing connection----");
        Serial.println();
    }
    clientLive.stop();
}