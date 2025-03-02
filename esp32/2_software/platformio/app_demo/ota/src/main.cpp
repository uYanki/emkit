

#include "SSD1306Wire.h"

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#endif

#include <ArduinoOTA.h>

const char* ssid     = "HUAWEI-Y6AZGD";
const char* password = "13631520360";

SSD1306Wire display(0x3c, SDA, SCL);  // ADDRESS, SDA, SCL

void setup()
{
    WiFi.begin(ssid, password);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(10);
    }

    display.init();
    display.flipScreenVertically();
    display.setContrast(255);

    ArduinoOTA.begin();
    ArduinoOTA.onStart([]() {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
        display.drawString(display.getWidth() / 2, display.getHeight() / 2 - 10, "OTA Update");
        display.display();
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        display.drawProgressBar(4, 32, 120, 8, progress / (total / 100));
        display.display();
    });

    ArduinoOTA.onEnd([]() {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
        display.drawString(display.getWidth() / 2, display.getHeight() / 2, "Restart");
        display.display();
    });

    // Align text vertical/horizontal center
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.setFont(ArialMT_Plain_10);
    display.drawString(display.getWidth() / 2, display.getHeight() / 2, "Ready for OTA:\n" + WiFi.localIP().toString());
    display.display();
}

void loop()
{
    ArduinoOTA.handle();
}
