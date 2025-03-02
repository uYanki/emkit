
#include "Arduino.h"

#define KEY0 34
#define KEY1 35
#define KEY2 36
#define KEY3 39

void setup()
{
    pinMode(KEY0, INPUT);
    pinMode(KEY1, INPUT);
    pinMode(KEY2, INPUT);
    pinMode(KEY3, INPUT);
    Serial.begin(115200);
    Serial.println("\r\n\r\nESP32_>>");
}

void loop()
{
    uint8_t u8KeySts = 0;
    u8KeySts |= (digitalRead(KEY0) == LOW) << 0;
    u8KeySts |= (digitalRead(KEY1) == LOW) << 1;
    u8KeySts |= (digitalRead(KEY2) == LOW) << 2;
    u8KeySts |= (digitalRead(KEY3) == LOW) << 3;
    Serial.println(u8KeySts);
    delay(300);
}