/*
 * @Author: zhaiyujia
 * @Date: 2022-05-11 17:49:50
 * @LastEditors: zhaiyujia
 * @LastEditTime: 2022-10-21 16:16:09
 * @Description: REMOTE_KEYBOARD_V1.1
 * [STM32F103C8XX]
 * 注意：由于STM32的RESET和BOOT0被ESP32用于ISP升级,当使用STLINK开发调试STM32时，需避免被ESP32干扰
 *  方法1：保证ESP32的程序中IO16、IO17是悬空状态
 *  方法2：通过按住板载按钮【BOOT】不放再按板载按钮【RST】使ESP32进入下载等待状态，此时IO16、IO17自然也被释放了。【此时板载绿灯处于微亮状态！】
 *
 */

#include <Arduino.h>
#include <Keyboard.h>

void setup()
{
    // Serial.begin(115200);
    // Serial.println("STM32F103C8XX_>>");
    Keyboard.begin();
    Keyboard.println("Hello STM32!");
}

void loop()
{
    Keyboard.println("Hello STM32!");
    delay(1000);
}