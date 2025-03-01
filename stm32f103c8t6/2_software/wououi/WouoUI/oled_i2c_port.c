#include "oled_port.h"

#define OLED_DEV_ADDR (0x3C << 1)

extern I2C_HandleTypeDef hi2c1;

// 推荐SPI方式,用I2C刷新动画还是太慢了

void OLED_Write_CMD(uint8_t data)
{
    HAL_I2C_Mem_Write(&hi2c1, OLED_DEV_ADDR, 0x00, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xFF);
}

void OLED_Write_DATA(uint8_t data)
{
    HAL_I2C_Mem_Write(&hi2c1, OLED_DEV_ADDR, 0x40, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xFF);
}

static void OLED_WriteByteArrayData(uint8_t* data_array, uint16_t len)
{
    HAL_I2C_Mem_Write(&hi2c1, OLED_DEV_ADDR, 0x40, I2C_MEMADD_SIZE_8BIT, data_array, len, 0xFF);
}

void OLED_Init(void)
{
    OLED_Write_CMD(0xAE);
    OLED_Write_CMD(0x00);
    OLED_Write_CMD(0x10);
    OLED_Write_CMD(0x40);
    OLED_Write_CMD(0x81);
    OLED_Write_CMD(0xCF);
    OLED_Write_CMD(0xA1);  // Set SEG/Column Mapping       0xa0左右反置 0xa1正常
    OLED_Write_CMD(0xC8);  // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_Write_CMD(0xA6);
    OLED_Write_CMD(0xA8);
    OLED_Write_CMD(0x3f);
    OLED_Write_CMD(0xD3);
    OLED_Write_CMD(0x00);
    OLED_Write_CMD(0xd5);
    OLED_Write_CMD(0x80);
    OLED_Write_CMD(0xD9);
    OLED_Write_CMD(0xF1);
    OLED_Write_CMD(0xDA);
    OLED_Write_CMD(0x12);
    OLED_Write_CMD(0xDB);
    OLED_Write_CMD(0x40);
    OLED_Write_CMD(0x20);
    OLED_Write_CMD(0x02);
    OLED_Write_CMD(0x8D);
    OLED_Write_CMD(0x14);
    OLED_Write_CMD(0xA4);
    OLED_Write_CMD(0xA6);
    OLED_Write_CMD(0xAF);
    OLED_Write_CMD(0xAF);  // Display ON
}

void OLED_SendBuff(uint8_t buff[8][128])
{
    for (uint8_t i = 0; i < 8; i++)
    {
        OLED_Write_CMD(0xb0 + i);               // 设置页地址（0~7）(b0-b7)
        OLED_Write_CMD(0x00);                   // set low column address
        OLED_Write_CMD(0x10);                   // 设置显示位置—列高地址
        OLED_WriteByteArrayData(buff[i], 128);  // 写一页128个字符
    }
}
