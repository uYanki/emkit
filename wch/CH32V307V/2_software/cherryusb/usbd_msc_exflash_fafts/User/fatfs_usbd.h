#ifndef __FATFS_PORT_FATFS_USBD_H__
#define __FATFS_PORT_FATFS_USBD_H__

#include "ff.h"

#define FLASH_START_ADDR  0x00040000 /*addr start from 256k */
#define FLASH_BLOCK_SIZE  4096
#define FLASH_BLOCK_COUNT 1024

int USB_disk_status(void);
int USB_disk_initialize(void);
int USB_disk_read(BYTE *buff, LBA_t sector, UINT count);
int USB_disk_write(const BYTE *buff, LBA_t sector, UINT count);
int USB_disk_ioctl(BYTE cmd, void *buff);

#endif /* __FATFS_PORT_FATFS_USBD_H__ */
