/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "usbd_core.h"
#include "usbd_hid.h"

/*!< hidraw in endpoint */
#define HIDRAW_IN_EP 0x81
#ifdef CONFIG_USB_HS
#define HIDRAW_IN_EP_SIZE  1024
#define HIDRAW_IN_INTERVAL 4
#else
#define HIDRAW_IN_EP_SIZE  64
#define HIDRAW_IN_INTERVAL 10
#endif
/*!< hidraw out endpoint */
#define HIDRAW_OUT_EP 0x02
#ifdef CONFIG_USB_HS
#define HIDRAW_OUT_EP_SIZE     1024
#define HIDRAW_OUT_EP_INTERVAL 4
#else
#define HIDRAW_OUT_EP_SIZE     64
#define HIDRAW_OUT_EP_INTERVAL 10
#endif

#define USBD_VID           0x34B7
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

/*!< config descriptor size */
#define USB_HID_CONFIG_DESC_SIZ (9 + 9 + 9 + 7 + 7)

/*!< custom hid report descriptor size */
#define HID_CUSTOM_REPORT_DESC_SIZE 38

/*!< global descriptor */
static const uint8_t hid_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0002, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_HID_CONFIG_DESC_SIZ, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    /************** Descriptor of Custom interface *****************/
    0x09,                          /* bLength: Interface Descriptor size */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType: Interface descriptor type */
    0x00,                          /* bInterfaceNumber: Number of Interface */
    0x00,                          /* bAlternateSetting: Alternate setting */
    0x02,                          /* bNumEndpoints */
    0x03,                          /* bInterfaceClass: HID */
    0x01,                          /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x00,                          /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,                             /* iInterface: Index of string descriptor */
    /******************** Descriptor of Custom HID ********************/
    0x09,                    /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE_HID, /* bDescriptorType: HID */
    0x11,                    /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,                        /* bCountryCode: Hardware target country */
    0x01,                        /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,                        /* bDescriptorType */
    HID_CUSTOM_REPORT_DESC_SIZE, /* wItemLength: Total length of Report descriptor */
    0x00,
    /******************** Descriptor of Custom in endpoint ********************/
    0x07,                         /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: */
    HIDRAW_IN_EP,                 /* bEndpointAddress: Endpoint Address (IN) */
    0x03,                         /* bmAttributes: Interrupt endpoint */
    WBVAL(HIDRAW_IN_EP_SIZE),     /* wMaxPacketSize: 4 Byte max */
    HIDRAW_IN_INTERVAL,           /* bInterval: Polling Interval */
    /******************** Descriptor of Custom out endpoint ********************/
    0x07,                         /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: */
    HIDRAW_OUT_EP,                /* bEndpointAddress: Endpoint Address (IN) */
    0x03,                         /* bmAttributes: Interrupt endpoint */
    WBVAL(HIDRAW_OUT_EP_SIZE),    /* wMaxPacketSize: 4 Byte max */
    HIDRAW_OUT_EP_INTERVAL,       /* bInterval: Polling Interval */
    /* 73 */
    /*
     * string0 descriptor
     */
    USB_LANGID_INIT(USBD_LANGID_STRING),
    /*
     * string1 descriptor
     */
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
    /*
     * string2 descriptor
     */
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
    'H', 0x00,                  /* wcChar10 */
    'I', 0x00,                  /* wcChar11 */
    'D', 0x00,                  /* wcChar12 */
    ' ', 0x00,                  /* wcChar13 */
    'D', 0x00,                  /* wcChar14 */
    'E', 0x00,                  /* wcChar15 */
    'M', 0x00,                  /* wcChar16 */
    'O', 0x00,                  /* wcChar17 */
    /*
     * string3 descriptor
     */
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
    /*
     * device qualifier descriptor
     */
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

/*!< custom hid report descriptor */
static const uint8_t hid_custom_report_desc[HID_CUSTOM_REPORT_DESC_SIZE] = {
#ifdef CONFIG_USB_HS
    /* USER CODE BEGIN 0 */
    0x06, 0x00, 0xff, /* USAGE_PAGE (Vendor Defined Page 1) */
    0x09, 0x01,       /* USAGE (Vendor Usage 1) */
    0xa1, 0x01,       /* COLLECTION (Application) */
    0x85, 0x02,       /*   REPORT ID (0x02) */
    0x09, 0x02,       /*   USAGE (Vendor Usage 1) */
    0x15, 0x00,       /*   LOGICAL_MINIMUM (0) */
    0x25, 0xff,       /*LOGICAL_MAXIMUM (255) */
    0x75, 0x08,       /*   REPORT_SIZE (8) */
    0x96, 0xff, 0x03, /*   REPORT_COUNT (63) */
    0x81, 0x02,       /*   INPUT (Data,Var,Abs) */
    /* <___________________________________________________> */
    0x85, 0x01,       /*   REPORT ID (0x01) */
    0x09, 0x03,       /*   USAGE (Vendor Usage 1) */
    0x15, 0x00,       /*   LOGICAL_MINIMUM (0) */
    0x25, 0xff,       /*   LOGICAL_MAXIMUM (255) */
    0x75, 0x08,       /*   REPORT_SIZE (8) */
    0x96, 0xff, 0x03, /*   REPORT_COUNT (63) */
    0x91, 0x02,       /*   OUTPUT (Data,Var,Abs) */
    /* USER CODE END 0 */
    0xC0 /*     END_COLLECTION	             */
#else
    /* USER CODE BEGIN 0 */
    0x06, 0x00, 0xff, /* USAGE_PAGE (Vendor Defined Page 1) */
    0x09, 0x01,       /* USAGE (Vendor Usage 1) */
    0xa1, 0x01,       /* COLLECTION (Application) */
    0x85, 0x02,       /*   REPORT ID (0x02) */
    0x09, 0x01,       /*   USAGE (Vendor Usage 1) */
    0x15, 0x00,       /*   LOGICAL_MINIMUM (0) */
    0x26, 0xff, 0x00, /*   LOGICAL_MAXIMUM (255) */
    0x95, 0x40 - 1,   /*   REPORT_COUNT (63) */
    0x75, 0x08,       /*   REPORT_SIZE (8) */
    0x81, 0x02,       /*   INPUT (Data,Var,Abs) */
    /* <___________________________________________________> */
    0x85, 0x01,       /*   REPORT ID (0x01) */
    0x09, 0x01,       /*   USAGE (Vendor Usage 1) */
    0x15, 0x00,       /*   LOGICAL_MINIMUM (0) */
    0x26, 0xff, 0x00, /*   LOGICAL_MAXIMUM (255) */
    0x95, 0x40 - 1,   /*   REPORT_COUNT (63) */
    0x75, 0x08,       /*   REPORT_SIZE (8) */
    0x91, 0x02,       /*   OUTPUT (Data,Var,Abs) */
    /* USER CODE END 0 */
    0xC0 /*     END_COLLECTION	             */
#endif
};

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer[HIDRAW_OUT_EP_SIZE];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t send_buffer[HIDRAW_IN_EP_SIZE];

#define HID_STATE_IDLE 0
#define HID_STATE_BUSY 1

/*!< hid state ! Data can be sent only when state is idle  */
static volatile uint8_t custom_state;

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
            /* setup first out ep read transfer */
            usbd_ep_start_read(busid, HIDRAW_OUT_EP, read_buffer, HIDRAW_OUT_EP_SIZE);
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;

        default:
            break;
    }
}

//
// i2c master ctrl
//

#include "stm32h7xx_hal.h"

extern I2C_HandleTypeDef hi2c1;

#define I2C_BASE      &hi2c1
#define le16(p)       *(uint16_t*)p

#define I2C_FUNC_TX   0
#define I2C_FUNC_RX   1
#define I2C_FUNC_WR   2
#define I2C_FUNC_RD   3
#define I2C_FUNC_SCAN 4

#define I2C_TIMEOUT   2

// #define I2C_FLAG_DEV_ADDR_10BIT (1 << 2)  /* this is a ten bit chip address */
// #define I2C_FLAG_NO_START       (1 << 4)  /* no start */
// #define I2C_FLAG_NO_STOP        (1 << 7)  /* no stop */
// #define I2C_FLAG_NO_RD_ACK      (1 << 6)  /* when I2C reading, we do not ACK */
// #define I2C_FLAG_CHK_WR_ACK     (1 << 8)  /* when I2C writing, need check the slave returns ack */
#define I2C_FLAG_MEM_ADDR_16BIT (1 << 15) /* this is a sixteen bit memory address */

static void usbd_hid_custom_in_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)busid;
    (void)ep;

    uint16_t u16Stat = 0;

    uint8_t* pInData  = &read_buffer[1];
    uint8_t* pOutData = &send_buffer[1];

    uint8_t  u8Operation;
    uint16_t u16DevAddr;
    uint16_t u16MemAddr;
    uint16_t u16MemAddrSize;
    uint8_t* pu8XferData;
    uint16_t u16XferSize;
    uint16_t u16XferFlags;

    memset(pOutData, 0, HIDRAW_OUT_EP_SIZE - 1);

    u8Operation = *pOutData++ = *pInData++;

    switch (u8Operation)
    {
        case I2C_FUNC_RX:
        {
            u16DevAddr = le16(pInData);
            pInData += 2;
            u16XferSize = le16(pInData);
            pInData += 2;
            // u16XferFlags = le16(pInData);
            pu8XferData = &pOutData[2];

            u16Stat = HAL_I2C_Master_Receive(I2C_BASE, u16DevAddr << 1, pu8XferData, u16XferSize, I2C_TIMEOUT) == HAL_OK;

            break;
        }

        case I2C_FUNC_TX:
        {
            u16DevAddr = le16(pInData);
            pInData += 2;
            u16XferSize = le16(pInData);
            pInData += 2;
            pu8XferData = pInData;
            pInData += u16XferSize;
            // u16XferFlags = le16(pInData);

            u16Stat = HAL_I2C_Master_Transmit(I2C_BASE, u16DevAddr << 1, pu8XferData, u16XferSize, I2C_TIMEOUT) == HAL_OK;

            break;
        }

        case I2C_FUNC_RD:
        {
            u16DevAddr = le16(pInData);
            pInData += 2;
            u16MemAddr = le16(pInData);
            pInData += 2;
            u16XferSize = le16(pInData);
            pInData += 2;
            u16XferFlags = le16(pInData);
            pu8XferData  = &pOutData[2];

            u16Stat = HAL_I2C_Mem_Read(I2C_BASE, u16DevAddr << 1, u16MemAddr, (u16XferFlags & I2C_FLAG_MEM_ADDR_16BIT) ? I2C_MEMADD_SIZE_16BIT : I2C_MEMADD_SIZE_8BIT, pu8XferData, u16XferSize, I2C_TIMEOUT) == HAL_OK;

            break;
        }

        case I2C_FUNC_WR:
        {
            u16DevAddr = le16(pInData);
            pInData += 2;
            u16MemAddr = le16(pInData);
            pInData += 2;
            u16XferSize = le16(pInData);
            pInData += 2;
            pu8XferData = pInData;
            pInData += u16XferSize;
            u16XferFlags = le16(pInData);

            u16Stat = HAL_I2C_Mem_Write(I2C_BASE, u16DevAddr << 1, u16MemAddr, (u16XferFlags & I2C_FLAG_MEM_ADDR_16BIT) ? I2C_MEMADD_SIZE_16BIT : I2C_MEMADD_SIZE_8BIT, pu8XferData, u16XferSize, I2C_TIMEOUT) == HAL_OK;

            break;
        }

        case I2C_FUNC_SCAN:
        {
            uint8_t* pu8ExitsAddr = &pOutData[2];
            uint8_t  u8RxData;
            uint8_t  u8Count = 0;

            for (u16DevAddr = 0; u16DevAddr < 127; u16DevAddr++)
            {
                u16Stat = HAL_I2C_IsDeviceReady(I2C_BASE, u16DevAddr << 1, 5, 1) == HAL_OK;

                if (u16Stat)
                {
                    pu8ExitsAddr[u16DevAddr / 8] |= (1 << u16DevAddr % 8);
                    u8Count++;
                }
            }

            u16Stat = u8Count;

            break;
        }
    }

    pOutData[0] = (u16Stat & 0xFF);
    pOutData[1] = (u16Stat >> 8);

    custom_state = HID_STATE_IDLE;
}

static void usbd_hid_custom_out_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    // USB_LOG_RAW("actual out len:%d\r\n", nbytes);
    usbd_ep_start_read(busid, ep, read_buffer, HIDRAW_IN_EP_SIZE);
    send_buffer[0] = 0x02; /* IN: report id */
    usbd_ep_start_write(busid, HIDRAW_IN_EP, send_buffer, nbytes);
}

static struct usbd_endpoint custom_in_ep = {
    .ep_cb   = usbd_hid_custom_in_callback,
    .ep_addr = HIDRAW_IN_EP};

static struct usbd_endpoint custom_out_ep = {
    .ep_cb   = usbd_hid_custom_out_callback,
    .ep_addr = HIDRAW_OUT_EP};

/* function ------------------------------------------------------------------*/
/**
 * @brief            hid custom init
 * @pre              none
 * @param[in]        none
 * @retval           none
 */
struct usbd_interface intf0;

void hid_custom_init(uint8_t busid, uintptr_t reg_base)
{
#ifdef CONFIG_USBDEV_ADVANCE_DESC
    usbd_desc_register(busid, &hid_descriptor);
#else
    usbd_desc_register(busid, hid_descriptor);
#endif
    usbd_add_interface(busid, usbd_hid_init_intf(busid, &intf0, hid_custom_report_desc, HID_CUSTOM_REPORT_DESC_SIZE));
    usbd_add_endpoint(busid, &custom_in_ep);
    usbd_add_endpoint(busid, &custom_out_ep);

    usbd_initialize(busid, reg_base, usbd_event_handler);
}
