#include "debug.h"
#include "ff.h"
#include "fftools.h"
#include "spi_flash.h"
#include "usbd_core.h"

/* Global typedef */

/* Global define */

/* Global Variable */

FATFS   fs;         // FatFs文件系统对象
FIL     fnew;       // 文件对象
FRESULT res_flash;  // 文件操作结果
DIR     dire;       // 目录对象
FILINFO fnow;       // 定义静态文件信息结构对象

UINT fnum;                    // 文件成功读写数量
BYTE ReadBuffer[1024] = {0};  // 读缓冲区
BYTE WriteBuffer[]    =       // 写缓冲区
    "测试: CH32V307X SPI_Flash + FatFs + USB MSC!";

BYTE work[FF_MAX_SS];  // FatFs缓冲区

extern void msc_init(uint8_t busid, uintptr_t reg_base);

unsigned char fftest(void);

void usb_dc_low_level_init(void)
{
    RCC_USBCLK48MConfig(RCC_USBCLK48MCLKSource_USBPHY);
    RCC_USBHSPLLCLKConfig(RCC_HSBHSPLLCLKSource_HSE);
    RCC_USBHSConfig(RCC_USBPLL_Div2);
    RCC_USBHSPLLCKREFCLKConfig(RCC_USBHSPLLCKREFCLK_4M);
    RCC_USBHSPHYPLLALIVEcmd(ENABLE);
#ifdef CONFIG_USB_HS
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBHS, ENABLE);
#else
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_OTG_FS, ENABLE);
#endif

    Delay_Us(100);
#ifndef CONFIG_USB_HS
    // EXTEN->EXTEN_CTR |= EXTEN_USBD_PU_EN;
    NVIC_EnableIRQ(OTG_FS_IRQn);
#else
    NVIC_EnableIRQ(USBHS_IRQn);
#endif
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    SPI_Flash_Init();

    printf("\r\nSystemCoreClock：%d\r\n", SystemCoreClock);

#if 1
    if( fftest() )
    {
        printf("FatFs test fail\r\n");
    }
#endif

    printf("usbhs flash disk %s\r\n", __TIME__);
    msc_init(0, USBHS_BASE);

    while (1)
    {
        // 逻辑代码
    }
}

unsigned char fftest(void)
{
    char pathBuff[256];  // 定义路径数组
    char filename[20];

    unsigned char ret = 0;

    res_flash = f_mount(&fs, "0:", 1);  // 挂载文件系统到分区 0

    if (res_flash == FR_NO_FILESYSTEM)  // 没有文件系统
    {
        printf("未发现文件系统，开始格式化: ");

        res_flash = f_mkfs("0:", 0, work, sizeof(work));  // 格式化 创建创建文件系统
        if (res_flash == FR_OK)
        {
            printf("格式化成功\r\n");
            res_flash = f_mount(NULL, "1:", 1);  // 格式化后，先取消挂载
            res_flash = f_mount(&fs, "1:", 1);   // 重新挂载
        }
        else
        {
            printf("格式化失败\r\n");
            ret = 1;  // 测试失败
        }
    }
    else if (res_flash == FR_OK)  // 挂载成功
    {
        printf("测试目录创建功能: ");

        strcpy(filename, "TestDir");
        res_flash = f_mkdir(filename);
        if (res_flash == FR_OK)
        {
            printf("成功创建文件夹[%s]\r\n", filename);
        }
        else if (res_flash == FR_EXIST)
        {
            printf("文件夹已存在\r\n");
        }
        else
        {
            printf("文件夹创建失败: %d\r\n", res_flash);
        }

        printf("测试文件写入功能: ");

        res_flash = f_open(&fnew, "0:TestDir/TestFile.txt", FA_CREATE_ALWAYS | FA_WRITE);  // 以写入方式打开文件，若未发现文件则新建文件
        if (res_flash == FR_OK)
        {
            res_flash = f_write(&fnew, WriteBuffer, strlen(WriteBuffer), &fnum);  // 写向文件内写入指定数据
            if (res_flash == FR_OK)
            {
                printf("成功写入[%d]字节数据:[%s]\r\n", fnum, WriteBuffer);
                f_close(&fnew);  // 关闭文件

                printf("测试文件读取功能: ");
                res_flash = f_open(&fnew, "0:TestDir/TestFile.txt", FA_OPEN_EXISTING | FA_READ);  // 重新打开
                if (res_flash == FR_OK)
                {
                    res_flash = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
                    if (res_flash == FR_OK)
                    {
                        printf("成功读取[%d]字节数据:[%s]\r\n", fnum, ReadBuffer);
                    }
                    else
                    {
                        printf("文件读取失败: %d\r\n", res_flash);
                        ret = 1;
                    }
                }
                else
                {
                    printf("打开文件失败\r\n");
                    ret = 1;
                }
                f_close(&fnew);
            }
            else
            {
                printf("向文件写入数据失败: %d\r\n", res_flash);
                f_close(&fnew);  // 关闭文件
                ret = 1;         // 测试失败
            }
        }
        else
        {
            printf("文件打开或者创建失败\r\n");
            ret = 1;  // 测试失败
        }

        printf("测试文件扫描功能: \r\n");
        strcpy(pathBuff, "");
        printf("-------------------------------------------\r\n");
        scan_files(pathBuff);
        printf("-------------------------------------------\r\n");
    }
    else
    {
        printf("文件系统挂载失败: %d\r\n", res_flash);
        printf("请检查 SPI Flash 工作状态\r\n");
        ret = 1;  // 测试失败
    }

    f_mount(NULL, "0:", 1);  // 卸载文件系统

    return ret;
}
