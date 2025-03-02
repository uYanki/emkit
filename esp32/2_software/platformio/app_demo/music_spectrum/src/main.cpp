// 快速傅里叶变换实现音乐频谱

#include <Arduino.h>
#include <arduinoFFT.h>
#include <Adafruit_SSD1306.h>

#define samples     128                           // 采样点数，2的N次幂
#define halfsamples samples / 2                   // 32
#define NumofCopy   halfsamples * sizeof(double)  // 位宽
#define Interval    128 / (halfsamples)           // 4
Adafruit_SSD1306 display(100);

double vReal[samples];
double vImag[samples];
double vTemp[halfsamples];

ArduinoFFT<double> FFT = ArduinoFFT<double>();

void setup()
{
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
}

void loop()
{
    // Serial.println("start:"+String(micros()));
    for (int i = 0; i < samples; i++)
    {
        vReal[i] = analogRead(36) - 512;  // 采样
        vImag[i] = 0.0;
    }
    // Serial.println("end:"+String(micros()));

    display.clearDisplay();
    FFT.windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  // 加汉宁窗
    FFT.compute(vReal, vImag, samples, FFT_FORWARD);                  // 计算fft
    FFT.complexToMagnitude(vReal, vImag, samples);                    // 计算幅度

    for (int i = 0; i < halfsamples - 2; i++)
    {
        display.drawPixel(i * Interval, vTemp[halfsamples - i - 1] * 0.007 + 1, WHITE);              // 点
        display.drawLine(i * Interval, 0, i * Interval, vReal[halfsamples - i - 1] * 0.007, WHITE);  // 线
    }
    display.display();

    memcpy(vTemp, vReal, NumofCopy);

    if (samples < 128)
    {
        delay(2);
    }
}
