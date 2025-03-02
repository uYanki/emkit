#include "diskio.h"
#include "string.h"
#include "spi_flash.h"
#include "fatfs_usbd.h"

int USB_disk_status(void)
{
    if( SPI_Flash_ReadID(  ) == DiskID )
    {
        return 0;
    }
    return 1;
}
int USB_disk_initialize(void)
{
    SPI_Flash_Init(  );
    return RES_OK;
}
int USB_disk_read(BYTE *buff, LBA_t sector, UINT count)
{
    SPI_Flash_Read( (uint8_t *)buff, FLASH_START_ADDR + sector * FLASH_BLOCK_SIZE, count );
    return 0;
}
int USB_disk_write(const BYTE *buff, LBA_t sector, UINT count)
{
    SPI_Flash_Erase_Sector(FLASH_START_ADDR + sector * FLASH_BLOCK_SIZE);
    SPI_Flash_Write( (uint8_t *)buff, FLASH_START_ADDR + sector * FLASH_BLOCK_SIZE, count);
    return 0;
}
int USB_disk_ioctl(BYTE cmd, void *buff)
{
    int result = 0;

    switch (cmd) {
        case CTRL_SYNC:
            result = RES_OK;
            break;

        case GET_SECTOR_SIZE:
            *(WORD *)buff = FLASH_BLOCK_SIZE;
            result = RES_OK;
            break;

        case GET_BLOCK_SIZE:
            *(DWORD *)buff = 1;
            result = RES_OK;
            break;

        case GET_SECTOR_COUNT:
            *(DWORD *)buff = FLASH_BLOCK_COUNT;
            result = RES_OK;
            break;

        default:
            result = RES_PARERR;
            break;
    }

    return result;
}
