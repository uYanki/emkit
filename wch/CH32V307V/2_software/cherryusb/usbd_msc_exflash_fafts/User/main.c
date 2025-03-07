#include "debug.h"
#include "ff.h"
#include "fftools.h"
#include "spi_flash.h"
#include "usbd_core.h"

/* Global typedef */

/* Global define */

/* Global Variable */

FATFS   fs;         // FatFs�ļ�ϵͳ����
FIL     fnew;       // �ļ�����
FRESULT res_flash;  // �ļ��������
DIR     dire;       // Ŀ¼����
FILINFO fnow;       // ���徲̬�ļ���Ϣ�ṹ����

UINT fnum;                    // �ļ��ɹ���д����
BYTE ReadBuffer[1024] = {0};  // ��������
BYTE WriteBuffer[]    =       // д������
    "����: CH32V307X SPI_Flash + FatFs + USB MSC!";

BYTE work[FF_MAX_SS];  // FatFs������

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

    printf("\r\nSystemCoreClock��%d\r\n", SystemCoreClock);

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
        // �߼�����
    }
}

unsigned char fftest(void)
{
    char pathBuff[256];  // ����·������
    char filename[20];

    unsigned char ret = 0;

    res_flash = f_mount(&fs, "0:", 1);  // �����ļ�ϵͳ������ 0

    if (res_flash == FR_NO_FILESYSTEM)  // û���ļ�ϵͳ
    {
        printf("δ�����ļ�ϵͳ����ʼ��ʽ��: ");

        res_flash = f_mkfs("0:", 0, work, sizeof(work));  // ��ʽ�� ���������ļ�ϵͳ
        if (res_flash == FR_OK)
        {
            printf("��ʽ���ɹ�\r\n");
            res_flash = f_mount(NULL, "1:", 1);  // ��ʽ������ȡ������
            res_flash = f_mount(&fs, "1:", 1);   // ���¹���
        }
        else
        {
            printf("��ʽ��ʧ��\r\n");
            ret = 1;  // ����ʧ��
        }
    }
    else if (res_flash == FR_OK)  // ���سɹ�
    {
        printf("����Ŀ¼��������: ");

        strcpy(filename, "TestDir");
        res_flash = f_mkdir(filename);
        if (res_flash == FR_OK)
        {
            printf("�ɹ������ļ���[%s]\r\n", filename);
        }
        else if (res_flash == FR_EXIST)
        {
            printf("�ļ����Ѵ���\r\n");
        }
        else
        {
            printf("�ļ��д���ʧ��: %d\r\n", res_flash);
        }

        printf("�����ļ�д�빦��: ");

        res_flash = f_open(&fnew, "0:TestDir/TestFile.txt", FA_CREATE_ALWAYS | FA_WRITE);  // ��д�뷽ʽ���ļ�����δ�����ļ����½��ļ�
        if (res_flash == FR_OK)
        {
            res_flash = f_write(&fnew, WriteBuffer, strlen(WriteBuffer), &fnum);  // д���ļ���д��ָ������
            if (res_flash == FR_OK)
            {
                printf("�ɹ�д��[%d]�ֽ�����:[%s]\r\n", fnum, WriteBuffer);
                f_close(&fnew);  // �ر��ļ�

                printf("�����ļ���ȡ����: ");
                res_flash = f_open(&fnew, "0:TestDir/TestFile.txt", FA_OPEN_EXISTING | FA_READ);  // ���´�
                if (res_flash == FR_OK)
                {
                    res_flash = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
                    if (res_flash == FR_OK)
                    {
                        printf("�ɹ���ȡ[%d]�ֽ�����:[%s]\r\n", fnum, ReadBuffer);
                    }
                    else
                    {
                        printf("�ļ���ȡʧ��: %d\r\n", res_flash);
                        ret = 1;
                    }
                }
                else
                {
                    printf("���ļ�ʧ��\r\n");
                    ret = 1;
                }
                f_close(&fnew);
            }
            else
            {
                printf("���ļ�д������ʧ��: %d\r\n", res_flash);
                f_close(&fnew);  // �ر��ļ�
                ret = 1;         // ����ʧ��
            }
        }
        else
        {
            printf("�ļ��򿪻��ߴ���ʧ��\r\n");
            ret = 1;  // ����ʧ��
        }

        printf("�����ļ�ɨ�蹦��: \r\n");
        strcpy(pathBuff, "");
        printf("-------------------------------------------\r\n");
        scan_files(pathBuff);
        printf("-------------------------------------------\r\n");
    }
    else
    {
        printf("�ļ�ϵͳ����ʧ��: %d\r\n", res_flash);
        printf("���� SPI Flash ����״̬\r\n");
        ret = 1;  // ����ʧ��
    }

    f_mount(NULL, "0:", 1);  // ж���ļ�ϵͳ

    return ret;
}
