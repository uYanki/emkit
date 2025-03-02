/*
 * @Author: zhaiyujia
 * @Date: 2022-07-10 15:24:57
 * @LastEditors: zhaiyujia
 * @LastEditTime: 2023-02-20 14:24:35
 * @Description: 以太网示例
 */

#include "Arduino.h"
#include <ETH.h>

#define LED 15
#define KEY 0

#define W5500_SCLK 18
#define W5500_MISO 19
#define W5500_MOSI 23
#define W5500_CS 5
#define W5500_RST 0 // BOOT按键

static bool eth_connected = false;

void WiFiEvent(WiFiEvent_t event)
{
    switch (event)
    {
    case ARDUINO_EVENT_ETH_START:
        Serial.println("ETH Started");
        // set eth hostname here
        ETH.setHostname("esp32-ethernet");
        break;
    case ARDUINO_EVENT_ETH_CONNECTED:
        Serial.println("ETH Connected");
        break;
    case ARDUINO_EVENT_ETH_GOT_IP:
        Serial.print("ETH MAC: ");
        Serial.print(ETH.macAddress());
        Serial.print(", IPv4: ");
        Serial.print(ETH.localIP());
        if (ETH.fullDuplex())
        {
            Serial.print(", FULL_DUPLEX");
        }
        Serial.print(", ");
        Serial.print(ETH.linkSpeed());
        Serial.println("Mbps");
        eth_connected = true;
        break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
        Serial.println("ETH Disconnected");
        eth_connected = false;
        break;
    case ARDUINO_EVENT_ETH_STOP:
        Serial.println("ETH Stopped");
        eth_connected = false;
        break;
    default:
        break;
    }
}

void testClient(const char *host, uint16_t port)
{
    Serial.print("\nconnecting to ");
    Serial.println(host);

    WiFiClient client;
    if (!client.connect(host, port))
    {
        Serial.println("connection failed");
        return;
    }
    client.printf("GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host);
    while (client.connected() && !client.available())
        ;
    while (client.available())
    {
        Serial.write(client.read());
    }

    Serial.println("closing connection\n");
    client.stop();
}

void setup()
{
    Serial.begin(115200);
    WiFi.onEvent(WiFiEvent);
    ETH.begin();
}

void loop()
{
    if (eth_connected)
    {
        testClient("baidu.com", 80);
    }
    delay(5000);
}
