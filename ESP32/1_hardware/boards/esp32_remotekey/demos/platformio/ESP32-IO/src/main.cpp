/*
 * @Author: zhaiyujia
 * @Date: 2022-07-10 15:24:57
 * @LastEditors: zhaiyujia
 * @LastEditTime: 2023-02-20 14:35:10
 * @Description: IO操作示例
 * 由于板载器件占用大部分IO，ESP32直接引出的IO仅：
 * IO04、IO32、IO33，此3个IO口均可作为输入、输出、PWM、ADC及TOUCH。
 */

#include "Arduino.h"
#define LED 15
#define KEY 0

// 板上丝印对应的IO
#define E04 4
#define E32 32
#define TBS 33

void setup()
{
    pinMode(KEY, INPUT);
    pinMode(LED, OUTPUT);
    pinMode(E04, OUTPUT);
    pinMode(E32, OUTPUT);
    pinMode(TBS, OUTPUT);
    Serial.begin(115200);
    Serial.println("\r\n\r\nESP32_>>");
}

void loop()
{
    digitalWrite(LED, !digitalRead(LED));
    digitalWrite(E04, !digitalRead(E04));
    digitalWrite(E32, !digitalRead(E32));
    digitalWrite(TBS, !digitalRead(TBS));
    Serial.println("Hello ESP32!");
    delay(1000);
}