// 注: 不推荐使用 iostream, 编译出来的 uf2 至少多 600kB

#include <stdio.h>
// #include <iostream>

#include "pico/stdlib.h"

#include "dht11.h"

#define DHT_PIN 14

int main()
{
    stdio_init_all();
    sleep_ms(2000);
    DHT11 dht11_sensor(DHT_PIN);
    while (1) {
        dht11_sensor.read();
        // std::cout << "Temp:" << dht11_sensor.m_temperature << " °C" << std::endl;
        // std::cout << "RH:" << dht11_sensor.m_humidity << " %" << std::endl;
        printf("%.4f\t%.4f\r\n", dht11_sensor.m_temperature, dht11_sensor.m_humidity);
        sleep_ms(200);
    }
    return 0;
}
