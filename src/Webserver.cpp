#include <Webserver.h>

Webserver::Webserver(Kamera *kamera, WiFiServer *server)
{
    this->kamera = kamera;
    this->server = server;
}

void Webserver::loop()
{
    get = "";
    url = "";
    WiFiClient client = server->available();
    WiFi.softAPIP();
    if (client)
    {

        Serial.println("new client");

        // an http request ends with a blank line

        bool currentLineIsBlank = true;

        while (client.connected())
        {
            if (client.available())
            {
                char c = client.read();
                Serial.write(c);
                get += c;
                if (get.length() > 40)
                {
                    get = get.substring(1);
                }

                if (get.endsWith(" HTTP/1.1"))
                {
                    url = get.substring(get.indexOf("GET ") + 4);
                    url.replace(" HTTP/1.1", "");
                    if (url.indexOf("?") > 0)
                    {
                        url = url.substring(0, url.indexOf("?"));
                    }
                }

                if (c == '\n' && currentLineIsBlank)
                {
                    Serial.print("url : ");
                    Serial.println(url);

                    client.println("HTTP/1.1 200 OK");
                    if (url.equals("/favicon.ico"))
                    {
                        client.println("Content-Type: image/x-icon");
                    }
                    else if (url.equals("/LiveView.jpg"))
                    {
                        client.println("Content-Type: image/jpeg");
                    }
                    else if (url.equals("/success.txt"))
                    {
                        client.println("Content-Type: text/plain");
                    }
                    else
                    {
                        client.println("Content-Type: text/html");
                    }
                    client.println("Connection: close"); // the connection will be closed after completion of the response
                    client.println();
                    if (url.equals("/click.html"))
                    {
                        kamera->click();
                        client.print(response("click"));
                    }
                    else if (url.equals("/success.txt"))
                    {
                        client.println("success");
                    }
                    else if (url.equals("/zoomIn.html"))
                    {
                        kamera->actZoomInS();
                        client.print(response("zoomIn"));
                    }
                    else if (url.equals("/zoomOut.html"))
                    {
                        kamera->actZoomOutS();
                        client.print(response("zoomOut"));
                    }
                    else if (url.equals("/motionOn.html"))
                    {
                        kamera->motionDeOn();
                        client.print(response("motionOn"));
                    }
                    else if (url.equals("/motionOff.html"))
                    {
                        kamera->motionDeOf();
                        client.print(response("motionOff"));
                    }
                    else if (url.equals("/favicon.ico"))
                    {
                        client.write(favicon, sizeof(favicon));
                    }
                    else if (url.equals("/LiveView.jpg"))
                    {
                        client.write(kamera->getJpeg(), kamera->getJpegSize());
                    }
                    else if (url.equals("/"))
                    {
                        client.println("<!DOCTYPE HTML>");
                        client.println("<html lang=\"de\">");
                        client.println("<head>");
                        client.println("<meta charset=\"utf-8\">");
                        client.println("<script>");
                        client.println("function refreshIt() {");
                        client.println("if (!document.images) return;");
                        client.println("document.images['LiveView'].src = 'LiveView.jpg' + \"?\" + new Date().getTime();");
                        client.println("setTimeout('refreshIt()', 5000);");
                        client.println("}");
                        client.println("function getUrl(strIn){");
                        client.println("var xhr = new XMLHttpRequest();");
                        client.println("xhr.onload = function() {");

                        client.println("}");
                        client.println("xhr.open('GET', strIn,true);");
                        client.println("xhr.send(null);");
                        client.println("}");

                        client.println("</script>");
                        client.println("</head>");
                        client.println("<body onLoad=\"refreshIt()\">");
                        client.println("Test Kamera ");
                        client.println("<br>");
                        client.println("<img src=\"LiveView.jpg\" name=\"LiveView\">");
                        client.println("<br />");
                        client.println("<button onclick=\"getUrl('click.html')\">Click</button>");
                        client.println("<button onclick=\"getUrl('zoomIn.html')\">zoom in</button>");
                        client.println("<button onclick=\"getUrl('zoomOut.html')\">zoom out</button>");
                        client.println("<button onclick=\"getUrl('motionOn.html')\">Motion De On</button>");
                        client.println("<button onclick=\"getUrl('motionOff.html')\">Motion De Off</button>");
                        client.println("<br>");

                        client.println("</html>");
                    }
                    break;
                }

                if (c == '\n')
                {
                    currentLineIsBlank = true;
                }
                else if (c != '\r')
                {
                    currentLineIsBlank = false;
                }
            }
        }

        client.flush();
        delay(5);
        client.stop();
        Serial.println("client disonnected");
    }
}
String Webserver::response(String in)
{
    String ret = "<!DOCTYPE HTML>\r\n";
    ret += "<html lang=\"de\">\r\n";
    ret += "<head>\r\n";
    ret += "<meta charset=\"utf-8\">\r\n";
    ret += in;
    ret += "\r\n";
    ret += "</html>\r\n";
    return ret;
}