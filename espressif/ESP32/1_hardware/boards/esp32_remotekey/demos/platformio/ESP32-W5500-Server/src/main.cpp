

#include "Arduino.h"
#include <SPI.h>
#include <Ethernet.h>

/**
 * 改动： Server.h => virtual void begin() = 0;
 */

#define LED         15
#define KEY         0

#define W5500_SCLK  18
#define W5500_MISO  19
#define W5500_MOSI  23
#define W5500_CS    5
#define W5500_RST   0  // BOOT按键

#define LED_BUILTIN LED

IPAddress      ip(192, 168, 137, 10);  // 192.168.137.10
EthernetServer server(80);
byte           mac[]            = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
boolean        alreadyConnected = false;  // whether or not the client was connected previously
uint16_t       xi               = 0;

void setup()
{
    Ethernet.init(SS);
    Serial.begin(115200);
    while (!Serial)
    {
        ;  // wait for serial port to connect. Needed for Leonardo only
    }
    delay(2000);
    Serial.println("COM0 setup OK!");

    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("LED_BUILTIN = " + String(LED_BUILTIN));
    Serial.println("SCK  = " + String(SCK));
    Serial.println("MISO = " + String(MISO));
    Serial.println("MOSI = " + String(MOSI));
    Serial.println("SS   = " + String(SS));

    Ethernet.begin(mac, ip);
    // start the server
    auto hwStatus = Ethernet.hardwareStatus();
    switch (hwStatus)
    {
        case EthernetNoHardware:
            Serial.println("No Ethernet hardware");
            break;
        case EthernetW5100:
            Serial.println("Ethernet W5100 installed");
            break;
        case EthernetW5200:
            Serial.println("Ethernet W5200 installed");
            break;
        case EthernetW5500:
            Serial.println("Ethernet W5500 installed");
            break;
    }

    auto link = Ethernet.linkStatus();
    Serial.print("Link status: ");
    switch (link)
    {
        case Unknown:
            Serial.println("Unknown");
            break;
        case LinkON:
            Serial.println("ON");
            break;
        case LinkOFF:
            Serial.println("OFF");
            break;
    }

    server.begin();
    Serial.print("server start at IP address:  ");
    Serial.println(Ethernet.localIP());
}

void loop()
{
    // wait for a new client:
    EthernetClient client = server.available();
    xi++;
    if (xi > 40000)
    {
        xi = 0;
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
    // when the client sends the first byte, say hello:
    if (client)
    {
        if (!alreadyConnected)
        {
            // clear out the input buffer:
            client.flush();
            Serial.println("We have a new client");
            client.println("Hello, client!");
            alreadyConnected = true;
        }
        if (client.available() > 0)
        {
            // read the bytes incoming from the client:
            char thisChar = client.read();
            // echo the bytes back to the client:
            server.write(thisChar);
            // echo the bytes to the server as well:
            Serial.write(thisChar);
        }
    }
}
