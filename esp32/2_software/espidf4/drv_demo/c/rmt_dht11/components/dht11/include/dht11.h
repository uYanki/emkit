#ifndef __DHT11_H__
#define __DHT11_H__

#include <soc/rmt_reg.h>
#include "driver/gpio.h"
#include "driver/rmt.h"  // 红外遥控
#include "esp_rom_sys.h"

#define DHT11_PIN 4

/**
 * @brief: 初始化
 */
void dht11_init();

/**
 * @brief: 获取温湿度
 * @param temperature 温度
 * @param humidity 湿度
 * @return:
 */
int dht11_start_get(int *temperature, int *humidity);

#endif