#include <Arduino.h>
#include <Ticker.h>

Ticker ticker;

void setup()
{
    Serial.begin(115200);
    
    ticker.attach(1, []() {
        Serial.printf("hello world\n");
    });
}

void loop()
{
}
