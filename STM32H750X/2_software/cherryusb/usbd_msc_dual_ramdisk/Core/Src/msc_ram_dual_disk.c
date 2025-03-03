/*
 * Copyright (c) 2024, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "usbd_core.h"
#include "usbd_msc.h"

#define MSC_IN_EP          0x81
#define MSC_OUT_EP         0x02

#define USBD_VID           0xFFFF
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#define USB_CONFIG_SIZE    (9 + MSC_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define MSC_MAX_MPS 512
#else
#define MSC_MAX_MPS 64
#endif

#ifdef CONFIG_USBDEV_ADVANCE_DESC
static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0200, 0x01)};

static const uint8_t config_descriptor[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    MSC_DESCRIPTOR_INIT(0x00, MSC_OUT_EP, MSC_IN_EP, MSC_MAX_MPS, 0x02)};

static const uint8_t device_quality_descriptor[] = {
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x00,
    0x00,
};

static const char* string_descriptors[] = {
    (const char[]){0x09, 0x04}, /* Langid */
    "CherryUSB", /* Manufacturer */
    "CherryUSB MSC DEMO", /* Product */
    "2022123456", /* Serial Number */
};

static const uint8_t* device_descriptor_callback(uint8_t speed)
{
    return device_descriptor;
}

static const uint8_t* config_descriptor_callback(uint8_t speed)
{
    return config_descriptor;
}

static const uint8_t* device_quality_descriptor_callback(uint8_t speed)
{
    return device_quality_descriptor;
}

static const char* string_descriptor_callback(uint8_t speed, uint8_t index)
{
    if (index > 3)
    {
        return NULL;
    }
    return string_descriptors[index];
}

const struct usb_descriptor msc_ram_descriptor = {
    .device_descriptor_callback         = device_descriptor_callback,
    .config_descriptor_callback         = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .string_descriptor_callback         = string_descriptor_callback};
#else
const uint8_t msc_ram_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0200, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    MSC_DESCRIPTOR_INIT(0x00, MSC_OUT_EP, MSC_IN_EP, MSC_MAX_MPS, 0x02),
    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
    0x14,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x26,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ' ', 0x00,                  /* wcChar9 */
    'M', 0x00,                  /* wcChar10 */
    'S', 0x00,                  /* wcChar11 */
    'C', 0x00,                  /* wcChar12 */
    ' ', 0x00,                  /* wcChar13 */
    'D', 0x00,                  /* wcChar14 */
    'E', 0x00,                  /* wcChar15 */
    'M', 0x00,                  /* wcChar16 */
    'O', 0x00,                  /* wcChar17 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '2', 0x00,                  /* wcChar3 */
    '1', 0x00,                  /* wcChar4 */
    '2', 0x00,                  /* wcChar5 */
    '3', 0x00,                  /* wcChar6 */
    '4', 0x00,                  /* wcChar7 */
    '5', 0x00,                  /* wcChar8 */
    '6', 0x00,                  /* wcChar9 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x00,
    0x00,
#endif
    0x00};
#endif

static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    switch (event)
    {
        case USBD_EVENT_RESET:
            break;
        case USBD_EVENT_CONNECTED:
            break;
        case USBD_EVENT_DISCONNECTED:
            break;
        case USBD_EVENT_RESUME:
            break;
        case USBD_EVENT_SUSPEND:
            break;
        case USBD_EVENT_CONFIGURED:
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;

        default:
            break;
    }
}

#define CONFIG_USBDEV_MSC_MAX_LUN 2

#define BLOCK1_SIZE               512
#define BLOCK1_COUNT              10
uint8_t mass_block1[BLOCK1_COUNT][BLOCK1_SIZE];

#if CONFIG_USBDEV_MSC_MAX_LUN > 1
#define BLOCK2_SIZE  512
#define BLOCK2_COUNT 10
uint8_t mass_block2[BLOCK2_COUNT][BLOCK2_SIZE];
#endif

void usbd_msc_get_cap(uint8_t busid, uint8_t lun, uint32_t* block_num, uint32_t* block_size)
{
    switch (lun)
    {
        case 0:
        {
            *block_num  = 1000;  // Pretend having so many buffer,not has actually.
            *block_size = BLOCK1_SIZE;
            break;
        }

#if CONFIG_USBDEV_MSC_MAX_LUN > 1
        case 1:
        {
            *block_num  = 2000;  // Pretend having so many buffer,not has actually.
            *block_size = BLOCK2_SIZE;
            break;
        }
#endif

        default:
        {
            *block_num  = 0;
            *block_size = 0;
            break;
        }
    }
}

int usbd_msc_sector_read(uint8_t busid, uint8_t lun, uint32_t sector, uint8_t* buffer, uint32_t length)
{
    switch (lun)
    {
        case 0:
        {
            if (sector < BLOCK1_COUNT)
            {
                memcpy(buffer, mass_block1[sector], length);
            }
            break;
        }

#if CONFIG_USBDEV_MSC_MAX_LUN > 1
        case 1:
        {
            if (sector < BLOCK2_COUNT)
            {
                memcpy(buffer, mass_block2[sector], length);
            }
            break;
        }
#endif

        default:
        {
            break;
        }
    }

    return 0;
}

int usbd_msc_sector_write(uint8_t busid, uint8_t lun, uint32_t sector, uint8_t* buffer, uint32_t length)
{
    switch (lun)
    {
        case 0:
        {
            if (sector < BLOCK1_COUNT)
            {
                memcpy(mass_block1[sector], buffer, length);
            }

            break;
        }

#if CONFIG_USBDEV_MSC_MAX_LUN > 1
        case 1:
        {
            if (sector < BLOCK2_COUNT)
            {
                memcpy(mass_block2[sector], buffer, length);
            }

            break;
        }
#endif
        default:
        {
            break;
        }
    }

    return 0;
}

static struct usbd_interface intf0;

void msc_ram_init(uint8_t busid, uintptr_t reg_base)
{
#ifdef CONFIG_USBDEV_ADVANCE_DESC
    usbd_desc_register(busid, &msc_ram_descriptor);
#else
    usbd_desc_register(busid, msc_ram_descriptor);
#endif
    usbd_add_interface(busid, usbd_msc_init_intf(busid, &intf0, MSC_OUT_EP, MSC_IN_EP));

    usbd_initialize(busid, reg_base, usbd_event_handler);
}

#if defined(CONFIG_USBDEV_MSC_POLLING)
void msc_ram_polling(uint8_t busid)
{
    usbd_msc_polling(busid);
}
#endif