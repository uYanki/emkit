#include "eefs_fileapi.h"
#include "eefs_filesys.h"
#include <string.h>
#include "hexdump.h"

#if CONFIG_DEMOS_SW

#define CONFIG_EEFS_IO          1  // 0:RAM,1:EEPROM

#define CONFIG_EEFS_DEVICE_NAME "/eefs"
#define CONFIG_EEFS_MOUNT_POINT "/disk"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "eefs"
#define LOG_LOCAL_LEVEL LOG_LEVEL_VERBOSE

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

#if CONFIG_EEFS_IO == 0

static uint8_t m_au8FileSystemBuffer[2048] = {0};  // RAW

#elif CONFIG_EEFS_IO == 1

#include "gdefs.h"
#include "hwif.h"
#include "i2c_eeprom.h"

static i2c_eeprom_t eefs_eeprom = {
    .hI2C      = nullptr,
    .u8SlvAddr = AT24CXX_ADDRESS_A000,
    .eCapacity = AT24CM01,
};

#endif

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/**
 * @brief write some data to RAM/EEPROM.
 *
 * @param[in]  Dest it's need must be make (void*) convetered to (int32)
 * @param[out] Src  it's the rom/ram appuse data area.
 *
 * @return none
 */

__weak void eefs_write(void* pDest, void* pSrc, int Length)
{
#if CONFIG_EEFS_IO == 0
    memcpy(&m_au8FileSystemBuffer[(int32_t)pDest], (int8_t*)pSrc, Length);
#elif CONFIG_EEFS_IO == 1
    EEPROM_WriteBlock(&eefs_eeprom, (int32_t)pDest, (int8_t*)pSrc, Length);

#if 1

    static uint8_t verify_buff[1024] = {0};

    HEXDUMP2(pSrc, Length);
    EEPROM_ReadBlock(&eefs_eeprom, (int32_t)pDest, verify_buff, Length);

    for (u16 i = 0; i < Length; i++)
    {
        if (verify_buff[i] != ((u8*)pSrc)[i])
        {
            LOGD("diff at %d", i);
            break;
        }
    }
#endif

#endif
}
/**
 *
 * @brief read some data from RAM/EEPROM.
 *
 * @param[out] Dest it's the rom/ram appuse data area.
 * @param[in]  Src  it's need must be make (void*) convetered to (int32)
 *
 * @return none
 */
__weak void eefs_read(void* pDest, void* pSrc, int Length)
{
#if CONFIG_EEFS_IO == 0
    memcpy((int8_t*)pDest, &m_au8FileSystemBuffer[(int32_t)pSrc], Length);
#elif CONFIG_EEFS_IO == 1
    EEPROM_ReadBlock(&eefs_eeprom, (int32_t)pSrc, (uint8_t*)pDest, Length);

    HEXDUMP2(pDest, Length);
#endif
}

static err_t EEFS_Setup(void)
{
    static EEFS_FileAllocationTable_t s_FileAllocationTable;

    /* Init eefs lib */
    EEFS_LibInit();

    /* Check the file allocation table and write repair if it does not exist. */
    EEFS_LIB_EEPROM_READ(&s_FileAllocationTable.Header, (void*)0, sizeof(EEFS_FileAllocationTableHeader_t));

    if ((s_FileAllocationTable.Header.Magic != EEFS_FILESYS_MAGIC) && (s_FileAllocationTable.Header.Version != 1))
    {
        /* EEFS_NO_SUCH_DEVICE */

        LOGD("reinit file system");

        /**
         * @note 此处 FileAllocationTable.Header.Version 的值固定为 1, 若需要更改为其他值，
         *       需屏蔽 EEFS_LibInitFS() 中的 `FileAllocationTableHeader.Version == 1` !!
         */

        /* Config file system */
        s_FileAllocationTable.Header.Crc              = 0;
        s_FileAllocationTable.Header.Magic            = EEFS_FILESYS_MAGIC;
        s_FileAllocationTable.Header.Version          = 1;  // fixed
        s_FileAllocationTable.Header.FreeMemoryOffset = sizeof(EEFS_FileAllocationTable_t);

#if CONFIG_EEFS_IO == 0
        s_FileAllocationTable.Header.FreeMemorySize = ARRAY_SIZE(m_au8FileSystemBuffer) - sizeof(EEFS_FileAllocationTable_t);
#elif CONFIG_EEFS_IO == 1
        s_FileAllocationTable.Header.FreeMemorySize = eefs_eeprom.eCapacity - sizeof(EEFS_FileAllocationTable_t);
        EEPROM_FillByte(&eefs_eeprom, 0, 1024 /* eefs_eeprom.eCapacity */, 0x00);  // 必须清空原有内容！！
#endif

        /* Write a file allocation table to eeprom. */
        eefs_write(0, &s_FileAllocationTable, sizeof(EEFS_FileAllocationTable_t));
    }

    /* Init eefs lib */

    if (EEFS_InitFS(CONFIG_EEFS_DEVICE_NAME, 0) != EEFS_SUCCESS)
    {
        LOGW("fail to init file system");
        return ERR_FAIL;  // fail to init eefs
    }

    if (EEFS_Mount(CONFIG_EEFS_DEVICE_NAME, CONFIG_EEFS_MOUNT_POINT) != EEFS_SUCCESS)
    {
        LOGW("fail to mount file system");
        return ERR_FAIL;  // fail to mount disk
    }

    LOGW("init success");

    return ERR_NONE;
}

err_t EEFS_Test(void)
{
#if CONFIG_EEFS_IO == 1

    // AC5测试通过(若不通过，则把软件I2C的频率调小!!), AC6无法通过测试

    static i2c_mst_t i2c = {
        .SDA  = {GPIOA, GPIO_PIN_6},
        .SCL  = {GPIOA, GPIO_PIN_5},
        .I2Cx = nullptr,
    };

    eefs_eeprom.hI2C = &i2c;

    I2C_Master_Init(&i2c, 1e5, I2C_DUTYCYCLE_50_50);
    I2C_Master_ScanAddress(&i2c);
    EEPROM_Init(&eefs_eeprom);

    // EEPROM_FillByte(&eefs_eeprom, 0, 1024, 0x00);

#endif

    ERRCHK_RETURN(EEFS_Setup());

    char*       szFileWrData     = "hello world!";
    static char szFileRdData[32] = {0};

    int32_t eefs_fd = EEFS_Creat("/disk/file.txt", 0);

    /* Write file test. */

    if (eefs_fd != EEFS_SUCCESS)
    {
        LOGW("fail to write file");
        return ERR_FAIL;  // fail to creat file
    }

    EEFS_Write(eefs_fd, (char*)szFileWrData, strlen(szFileWrData));  // return WrCnt

    EEFS_Close(eefs_fd);

    /* Read file test. */

    if (EEFS_Open("/disk/file.txt", 0) != EEFS_SUCCESS)
    {
        LOGW("fail to read file");
        return ERR_FAIL;  // fail to open file
    }

    memset(szFileRdData, 0, sizeof(szFileRdData));
    EEFS_Read(eefs_fd, szFileRdData, sizeof(szFileRdData));  // return RdCnt

    LOGD("%s", szFileRdData);

    EEFS_Close(eefs_fd);

    return ERR_NONE;
}

#endif
