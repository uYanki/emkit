/*
 * @Author: zhaiyujia
 * @Date: 2022-07-10 15:24:57
 * @LastEditors: zhaiyujia
 * @LastEditTime: 2023-02-20 14:46:10
 * @Description: HC595串入并出扩展IO
 * 板载3片HC595对应的引脚丝印：K01~K24共24个脚
 * ShiftRegister74HC595：https://github.com/Simsso/ShiftRegister74HC595
 */

#include "Arduino.h"
#include "ShiftRegister74HC595.h"
#define LED 15
#define KEY 0

#define HC595_SI 13
#define HC595_SCK 14
#define HC595_RCK 27
#define HC595_PCS 3 // HC595级联片数

ShiftRegister74HC595<HC595_PCS> hc595(HC595_SI, HC595_SCK, HC595_RCK);

void setup()
{
    pinMode(LED, OUTPUT);
    Serial.begin(115200);
    Serial.println("\r\n\r\nESP32_>>");
}

void loop()
{
    // 统一控制
    hc595.setAllHigh();
    delay(500);
    hc595.setAllLow();
    delay(500);

    // 轮流控制
    for (int i = 0; i < 8; i++)
    {
        hc595.set(i, HIGH);
        delay(250);
    }

    // 按位设置引脚状态
    uint8_t pinValues[] = {B10101010};
    hc595.setAll(pinValues);
    delay(1000);

    // 获取、设置引脚状态
    uint8_t stateOfPin5 = hc595.get(5);
    hc595.set(6, stateOfPin5);

    // 设置引脚数据但不立即更新
    hc595.setNoUpdate(0, HIGH);
    hc595.setNoUpdate(1, LOW);
    hc595.updateRegisters(); // 将上面设置的数据更新到器件
}