/*

 * 这个头文件是oled库的 [硬件层] 实现文件，移植的时候需要更改这个文件的内容

*/

#include "OLED_driver.h"
#define OLED_DEV_ADDR (0x3C << 1)

uint8_t OLED_DisplayBuf[64 / 8][128];
bool    OLED_ColorMode = true;

extern I2C_HandleTypeDef hi2c1;

void OLED_Write_DATA(uint8_t data)
{
#if OLED_IF_MODE == OLED_IF_HWI2C

    HAL_I2C_Mem_Write(&hi2c1, OLED_DEV_ADDR, 0x40, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xFF);

#elif OLED_IF_MODE == OLED_IF_SWSPI
    OLED_DC_Set();  // 设置数据命令线为数据模式
    OLED_CS_Clr();  // 选中OLED

    for (uint8_t i = 0; i < 8; i++)
    {
        OLED_SCL_Clr();   // 时钟线置低，准备发送数据位
        if (data & 0x80)  // 检查最高位
        {
            OLED_SDA_Set();  // 如果是1，设置数据线高
        }
        else
        {
            OLED_SDA_Clr();  // 如果是0，设置数据线低
        }

        OLED_SCL_Set();  // 时钟线置高，数据线状态被读取
        data <<= 1;      // 左移数据准备下一位
    }

    OLED_CS_Set();  // 取消选中OLED
#endif
}

/**
 * 函    数：OLED写数据
 * 参    数：Data 要写入数据的起始地址
 * 参    数：Count 要写入数据的数量
 * 返 回 值：无
 */
void OLED_WriteDataArr(uint8_t* Data, uint8_t Count)
{
#if OLED_IF_MODE == OLED_IF_HWI2C

    HAL_I2C_Mem_Write(&hi2c1, OLED_DEV_ADDR, 0x40, I2C_MEMADD_SIZE_8BIT, Data, Count, 0xFF);

#elif OLED_IF_MODE == OLED_IF_SWSPI

    OLED_DC_Set();  // 设置数据命令线为数据模式
    OLED_CS_Clr();  // 选中OLED

    /*循环Count次，进行连续的数据写入*/
    for (uint8_t i = 0; i < Count; i++)
    {
        if (OLED_ColorMode)
        {
            OLED_Write_DATA(Data[i]);  // 依次发送Data的每一个数据
        }
        else
        {
            OLED_Write_DATA(~Data[i]);  // 如果是反色模式，则将每个数据取反再发送
        }
    }
    OLED_CS_Set();  // 取消选中OLED

#endif
}

void OLED_Write_CMD(uint8_t data)
{
#if OLED_IF_MODE == OLED_IF_HWI2C
    HAL_I2C_Mem_Write(&hi2c1, OLED_DEV_ADDR, 0x00, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xFF);
#elif OLED_IF_MODE == OLED_IF_SWSPI
    OLED_DC_Clr();
    OLED_CS_Clr();
    for (uint8_t i = 0; i < 8; i++)
    {
        OLED_SCL_Clr();
        if (data & 0x80)
        {
            OLED_SDA_Set();
        }
        else
        {
            OLED_SDA_Clr();
        }
        OLED_SCL_Set();
        data <<= 1;
    }
    OLED_CS_Set();
#endif
}

// 反显函数
void OLED_ColorTurn(uint8_t i)
{
    if (i == 0)
    {
        OLED_Write_CMD(0xA6);  // 正常显示
    }
    if (i == 1)
    {
        OLED_Write_CMD(0xA7);  // 反色显示
    }
}

// 开启OLED显示
void OLED_DisPlay_On(void)
{
    OLED_Write_CMD(0x8D);  // 电荷泵使能
    OLED_Write_CMD(0x14);  // 开启电荷泵
    OLED_Write_CMD(0xAF);  // 点亮屏幕
}

// 关闭OLED显示
void OLED_DisPlay_Off(void)
{
    OLED_Write_CMD(0x8D);  // 电荷泵使能
    OLED_Write_CMD(0x10);  // 关闭电荷泵
    OLED_Write_CMD(0xAE);  // 关闭屏幕
}
/**
 * 函    数：OLED设置显示光标位置
 * 参    数：Page 指定光标所在的页，范围：0~15
 * 参    数：X 指定光标所在的X轴坐标，范围：0~127
 * 返 回 值：无
 * 说    明：OLED默认的Y轴，只能8个Bit为一组写入，即1页等于8个Y轴坐标
 */
void OLED_SetCursor(uint8_t Page, uint8_t X)
{
    /*如果使用此程序驱动1.3寸的OLED显示屏，则需要解除此注释*/
    /*因为1.3寸的OLED驱动芯片（SH1106）有132列*/
    /*屏幕的起始列接在了第2列，而不是第0列*/
    /*所以需要将X加2，才能正常显示*/
    //	X += 2;

    /*通过指令设置页地址和列地址*/
    OLED_Write_CMD(0xB0 | Page);               // 设置页位置
    OLED_Write_CMD(0x10 | ((X & 0xF0) >> 4));  // 设置X位置高4位
    OLED_Write_CMD(0x00 | (X & 0x0F));         // 设置X位置低4位
}

// 更新显存到OLED
void OLED_Update(void)
{
    uint8_t j;
    /*遍历每一页*/
    for (j = 0; j < 8; j++)
    {
        /*设置光标位置为每一页的第一列*/
        OLED_SetCursor(j, 0);
        /*连续写入128个数据，将显存数组的数据写入到OLED硬件*/
        OLED_WriteDataArr(OLED_DisplayBuf[j], 128);
    }
}
/**
 * 函    数：将OLED显存数组部分更新到OLED屏幕
 * 参    数：X 指定区域左上角的横坐标，范围：0~127
 * 参    数：Y 指定区域左上角的纵坐标，范围：0~127
 * 参    数：Width 指定区域的宽度，范围：0~128
 * 参    数：Height 指定区域的高度，范围：0~127
 * 返 回 值：无
 * 说    明：此函数会至少更新参数指定的区域
 *           如果更新区域Y轴只包含部分页，则同一页的剩余部分会跟随一起更新
 * 说    明：所有的显示函数，都只是对OLED显存数组进行读写
 *           随后调用OLED_Update函数或OLED_UpdateArea函数
 *           才会将显存数组的数据发送到OLED硬件，进行显示
 *           故调用显示函数后，要想真正地呈现在屏幕上，还需调用更新函数
 */

void OLED_UpdateArea(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height)
{
    uint8_t j;

    /*参数检查，保证指定区域不会超出屏幕范围*/
    if (X > 128 - 1) { return; }
    if (Y > 64 - 1) { return; }
    if (X + Width > 128) { Width = 128 - X; }
    if (Y + Height > 64) { Height = 64 - Y; }

    /*遍历指定区域涉及的相关页*/
    /*(Y + Height - 1) / 8 + 1的目的是(Y + Height) / 8并向上取整*/
    for (j = Y / 8; j < (Y + Height - 1) / 8 + 1; j++)
    {
        /*设置光标位置为相关页的指定列*/
        OLED_SetCursor(j, X);
        /*连续写入Width个数据，将显存数组的数据写入到OLED硬件*/
        OLED_WriteDataArr(&OLED_DisplayBuf[j][X], Width);
    }
}
extern void OLED_Clear(void);

// OLED的初始化
void OLED_Init(void)
{
#if OLED_IF_MODE == OLED_IF_SWSPI
    OLED_RES_Clr();
#endif
    for (int i = 0; i < 1000; i++)
    {
        for (int j = 0; j < 1000; j++)
        {
        }
    }
#if OLED_IF_MODE == OLED_IF_SWSPI
    OLED_RES_Set();
#endif

    OLED_Write_CMD(0xAE);
    OLED_Write_CMD(0x00);
    OLED_Write_CMD(0x10);
    OLED_Write_CMD(0x40);
    OLED_Write_CMD(0x81);
    OLED_Write_CMD(0xCF);
    OLED_Write_CMD(0xA1);
    OLED_Write_CMD(0xC8);
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
    OLED_Clear();
    OLED_Write_CMD(0xAF);  // Display ON
    for (int i = 0; i < 1000; i++)
    {
        for (int j = 0; j < 1000; j++)
        {
        }
    }
}

/**
 * 函    数：OLED设置亮度
 * 参    数：Brightness ，0-255，不同显示芯片效果可能不相同。
 * 返 回 值：无
 * 说    明：不要设置过大或者过小。
 */
void OLED_Brightness(int16_t Brightness)
{
    if (Brightness > 255)
    {
        Brightness = 255;
    }
    if (Brightness < 0)
    {
        Brightness = 0;
    }
    OLED_Write_CMD(0x81);
    OLED_Write_CMD(Brightness);
}

/**
 * @brief 设置显示模式
 * @param colormode true: 黑色模式，false: 白色模式
 * @return 无
 */
void OLED_SetColorMode(bool colormode)
{
    OLED_ColorMode = colormode;
}
