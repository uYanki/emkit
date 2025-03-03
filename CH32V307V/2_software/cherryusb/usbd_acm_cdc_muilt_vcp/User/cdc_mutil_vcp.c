/*
 * Copyright (c) 2024, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "usbd_core.h"
#include "usbd_cdc_acm.h"

#define CDC_PORT_NUM 7

/*!< endpoint address */
#define CDC_IN_EP          0x81
#define CDC_OUT_EP         0x01
#define CDC_INT_EP         0x88

#define CDC_IN_EP2         0x82
#define CDC_OUT_EP2        0x02
#define CDC_INT_EP2        0x89

#define CDC_IN_EP3         0x83
#define CDC_OUT_EP3        0x03
#define CDC_INT_EP3        0x8A

#define CDC_IN_EP4         0x84
#define CDC_OUT_EP4        0x04
#define CDC_INT_EP4        0x8B

#define CDC_IN_EP5         0x85
#define CDC_OUT_EP5        0x05
#define CDC_INT_EP5        0x8C

#define CDC_IN_EP6         0x86
#define CDC_OUT_EP6        0x06
#define CDC_INT_EP6        0x8D

#define CDC_IN_EP7         0x87
#define CDC_OUT_EP7        0x07
#define CDC_INT_EP7        0x8E

#define USBD_VID           0xFFFF
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

/*!< config descriptor size */
#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN * CDC_PORT_NUM)

#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif

/*!< global descriptor */
static const uint8_t cdc_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, (CDC_PORT_NUM << 1), 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_MAX_MPS, 0x02),

#if (CDC_PORT_NUM > 1)
    CDC_ACM_DESCRIPTOR_INIT(2, CDC_INT_EP2, CDC_OUT_EP2, CDC_IN_EP2, CDC_MAX_MPS, 0x0),
#endif
#if (CDC_PORT_NUM > 2)
    CDC_ACM_DESCRIPTOR_INIT(4, CDC_INT_EP3, CDC_OUT_EP3, CDC_IN_EP3, CDC_MAX_MPS, 0x0),
#endif
#if (CDC_PORT_NUM > 3)
    CDC_ACM_DESCRIPTOR_INIT(6, CDC_INT_EP4, CDC_OUT_EP4, CDC_IN_EP4, CDC_MAX_MPS, 0x0),
#endif
#if (CDC_PORT_NUM > 4)
    CDC_ACM_DESCRIPTOR_INIT(8, CDC_INT_EP5, CDC_OUT_EP5, CDC_IN_EP5, CDC_MAX_MPS, 0x0),
#endif
#if (CDC_PORT_NUM > 5)
    CDC_ACM_DESCRIPTOR_INIT(10, CDC_INT_EP6, CDC_OUT_EP6, CDC_IN_EP6, CDC_MAX_MPS, 0x0),
#endif
#if (CDC_PORT_NUM > 6)
    CDC_ACM_DESCRIPTOR_INIT(12, CDC_INT_EP7, CDC_OUT_EP7, CDC_IN_EP7, CDC_MAX_MPS, 0x0),
#endif

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
    'C', 0x00,                  /* wcChar10 */
    'D', 0x00,                  /* wcChar11 */
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
    0x00,
};

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer[CDC_PORT_NUM][1024]; /* 1024 is only for test speed , please use CDC_MAX_MPS for common*/
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t write_buffer[CDC_PORT_NUM][1024];

volatile uint8_t cdc_port_tx_busy[CDC_PORT_NUM];

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

            usbd_ep_start_read(busid, CDC_OUT_EP, read_buffer[0], 1024);
#if (CDC_PORT_NUM > 1)
            usbd_ep_start_read(busid, CDC_OUT_EP2, read_buffer[1], 1024);
#endif
#if (CDC_PORT_NUM > 2)
            usbd_ep_start_read(busid, CDC_OUT_EP3, read_buffer[2], 1024);
#endif
#if (CDC_PORT_NUM > 3)
            usbd_ep_start_read(busid, CDC_OUT_EP4, read_buffer[3], 1024);
#endif
#if (CDC_PORT_NUM > 4)
            usbd_ep_start_read(busid, CDC_OUT_EP5, read_buffer[4], 1024);
#endif
#if (CDC_PORT_NUM > 5)
            usbd_ep_start_read(busid, CDC_OUT_EP6, read_buffer[5], 1024);
#endif
#if (CDC_PORT_NUM > 6)
            usbd_ep_start_read(busid, CDC_OUT_EP7, read_buffer[6], 1024);
#endif

            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;

        default:
            break;
    }
}

void usbd_cdc_acm_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    uint8_t port;
    uint8_t ep_in;

    USB_LOG_RAW("EP_%02X actual out len:%d\r\n", ep, nbytes);

    // for (int i = 0; i < 100; i++) {
    //     printf("%02x ", read_buffer[i]);
    // }
    // printf("\r\n");
    /* setup next out ep read transfer */

    switch (ep)
    {
        case CDC_OUT_EP:
            port  = 0;
            ep_in = CDC_IN_EP;
            break;
#if (CDC_PORT_NUM > 1)
        case CDC_OUT_EP2:
            port  = 1;
            ep_in = CDC_IN_EP2;
            break;
#endif
#if (CDC_PORT_NUM > 2)
        case CDC_OUT_EP3:
            port  = 2;
            ep_in = CDC_IN_EP3;
            break;
#endif
#if (CDC_PORT_NUM > 3)
        case CDC_OUT_EP4:
            port  = 3;
            ep_in = CDC_IN_EP4;
            break;
#endif
#if (CDC_PORT_NUM > 4)
        case CDC_OUT_EP5:
            port  = 4;
            ep_in = CDC_IN_EP5;
            break;
#endif
#if (CDC_PORT_NUM > 5)
        case CDC_OUT_EP6:
            port  = 5;
            ep_in = CDC_IN_EP6;
            break;
#endif
#if (CDC_PORT_NUM > 6)
        case CDC_OUT_EP7:
            port  = 6;
            ep_in = CDC_IN_EP7;
            break;
#endif
        default:
            USB_LOG_ERR("Unknown out EP(0x%X) data\r\n", ep);
            return;
    }
    if (0 == cdc_port_tx_busy[port])
    {
        cdc_port_tx_busy[port] = 1;
        memcpy(write_buffer[port], read_buffer[port], nbytes);
        usbd_ep_start_write(busid, ep_in, write_buffer[port], nbytes);
    }
    usbd_ep_start_read(busid, ep, read_buffer[port], 1024);
}

void usbd_cdc_acm_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    uint8_t port;

    USB_LOG_RAW("actual in len:%d\r\n", nbytes);

    if ((nbytes % CDC_MAX_MPS) == 0 && nbytes)
    {
        /* send zlp */
        // usbd_ep_start_write(CDC_IN_EP, NULL, 0);
        usbd_ep_start_write(busid, ep, NULL, 0);
    }
    else
    {
        // ep_tx_busy_flag = false;
        switch (ep)
        {
            case CDC_IN_EP: port = 0; break;
#if (CDC_PORT_NUM > 1)
            case CDC_IN_EP2: port = 1; break;
#endif
#if (CDC_PORT_NUM > 2)
            case CDC_IN_EP3: port = 2; break;
#endif
#if (CDC_PORT_NUM > 3)
            case CDC_IN_EP4: port = 3; break;
#endif
#if (CDC_PORT_NUM > 4)
            case CDC_IN_EP5: port = 4; break;
#endif
#if (CDC_PORT_NUM > 5)
            case CDC_IN_EP6: port = 5; break;
#endif
#if (CDC_PORT_NUM > 6)
            case CDC_IN_EP7: port = 6; break;
#endif
            default:
                USB_LOG_ERR("Unknown in EP(0x%X) data\r\n", ep);
                return;
        }
        cdc_port_tx_busy[port] = 0;
    }
}

/*!< endpoint call back */
struct usbd_endpoint cdc_out_ep = {
    .ep_addr = CDC_OUT_EP,
    .ep_cb   = usbd_cdc_acm_bulk_out};

struct usbd_endpoint cdc_in_ep = {
    .ep_addr = CDC_IN_EP,
    .ep_cb   = usbd_cdc_acm_bulk_in};

static struct usbd_interface intf0;
static struct usbd_interface intf1;

#if (CDC_PORT_NUM > 1)

struct usbd_interface intf2;
struct usbd_interface intf3;

struct usbd_endpoint cdc_out_ep2 = {
    .ep_addr = CDC_OUT_EP2,
    .ep_cb   = usbd_cdc_acm_bulk_out};

struct usbd_endpoint cdc_in_ep2 = {
    .ep_addr = CDC_IN_EP2,
    .ep_cb   = usbd_cdc_acm_bulk_in};
#endif

#if (CDC_PORT_NUM > 2)

struct usbd_interface intf4;
struct usbd_interface intf5;

struct usbd_endpoint cdc_out_ep3 = {
    .ep_addr = CDC_OUT_EP3,
    .ep_cb   = usbd_cdc_acm_bulk_out};

struct usbd_endpoint cdc_in_ep3 = {
    .ep_addr = CDC_IN_EP3,
    .ep_cb   = usbd_cdc_acm_bulk_in};
#endif

#if (CDC_PORT_NUM > 3)

struct usbd_interface intf6;
struct usbd_interface intf7;

struct usbd_endpoint cdc_out_ep4 = {
    .ep_addr = CDC_OUT_EP4,
    .ep_cb   = usbd_cdc_acm_bulk_out};

struct usbd_endpoint cdc_in_ep4 = {
    .ep_addr = CDC_IN_EP4,
    .ep_cb   = usbd_cdc_acm_bulk_in};
#endif

#if (CDC_PORT_NUM > 4)

struct usbd_interface intf8;
struct usbd_interface intf9;

struct usbd_endpoint cdc_out_ep5 = {
    .ep_addr = CDC_OUT_EP5,
    .ep_cb   = usbd_cdc_acm_bulk_out};

struct usbd_endpoint cdc_in_ep5 = {
    .ep_addr = CDC_IN_EP5,
    .ep_cb   = usbd_cdc_acm_bulk_in};
#endif

#if (CDC_PORT_NUM > 5)

struct usbd_interface intf10;
struct usbd_interface intf11;

struct usbd_endpoint cdc_out_ep6 = {
    .ep_addr = CDC_OUT_EP6,
    .ep_cb   = usbd_cdc_acm_bulk_out};

struct usbd_endpoint cdc_in_ep6 = {
    .ep_addr = CDC_IN_EP6,
    .ep_cb   = usbd_cdc_acm_bulk_in};
#endif

#if (CDC_PORT_NUM > 6)

struct usbd_interface intf12;
struct usbd_interface intf13;

struct usbd_endpoint cdc_out_ep7 = {
    .ep_addr = CDC_OUT_EP7,
    .ep_cb   = usbd_cdc_acm_bulk_out};

struct usbd_endpoint cdc_in_ep7 = {
    .ep_addr = CDC_IN_EP7,
    .ep_cb   = usbd_cdc_acm_bulk_in};
#endif

struct cdc_line_coding com_cfg[CDC_PORT_NUM];

void usbd_cdc_acm_set_line_coding(uint8_t busid, uint8_t intf, struct cdc_line_coding* line_coding)
{
    if (intf >= (CDC_PORT_NUM << 1))
    {
        return;
    }

    memcpy(&com_cfg[intf / 2], line_coding, sizeof(struct cdc_line_coding));
}

void usbd_cdc_acm_get_line_coding(uint8_t busid, uint8_t intf, struct cdc_line_coding* line_coding)
{
    if (intf >= (CDC_PORT_NUM << 1))
    {
        return;
    }
    memcpy(line_coding, &com_cfg[intf / 2], sizeof(struct cdc_line_coding));
}

void cdc_acm_init(uint8_t busid, uintptr_t reg_base)
{
    for (int i = 0; i < CDC_PORT_NUM; i++)
    {
        com_cfg[i].dwDTERate   = 115200;
        com_cfg[i].bDataBits   = 8;
        com_cfg[i].bParityType = 0;
        com_cfg[i].bCharFormat = 0;

        cdc_port_tx_busy[i] = 0;
    }

#ifdef CONFIG_USBDEV_ADVANCE_DESC
    usbd_desc_register(busid, &cdc_descriptor);
#else
    usbd_desc_register(busid, cdc_descriptor);
#endif

    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf0));
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf1));
    usbd_add_endpoint(busid, &cdc_out_ep);
    usbd_add_endpoint(busid, &cdc_in_ep);

#if (CDC_PORT_NUM > 1)
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf2));
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf3));
    usbd_add_endpoint(busid, &cdc_out_ep2);
    usbd_add_endpoint(busid, &cdc_in_ep2);
#endif

#if (CDC_PORT_NUM > 2)
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf4));
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf5));
    usbd_add_endpoint(busid, &cdc_out_ep3);
    usbd_add_endpoint(busid, &cdc_in_ep3);
#endif

#if (CDC_PORT_NUM > 3)
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf6));
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf7));
    usbd_add_endpoint(busid, &cdc_out_ep4);
    usbd_add_endpoint(busid, &cdc_in_ep4);
#endif

#if (CDC_PORT_NUM > 4)
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf8));
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf9));
    usbd_add_endpoint(busid, &cdc_out_ep5);
    usbd_add_endpoint(busid, &cdc_in_ep5);
#endif

#if (CDC_PORT_NUM > 5)
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf10));
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf11));
    usbd_add_endpoint(busid, &cdc_out_ep6);
    usbd_add_endpoint(busid, &cdc_in_ep6);
#endif

#if (CDC_PORT_NUM > 6)
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf12));
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf13));
    usbd_add_endpoint(busid, &cdc_out_ep7);
    usbd_add_endpoint(busid, &cdc_in_ep7);
#endif

    usbd_initialize(busid, reg_base, usbd_event_handler);
}

#if 0

volatile uint8_t dtr_enable = 0;

void usbd_cdc_acm_set_dtr(uint8_t busid, uint8_t intf, bool dtr)
{
    if (dtr)
    {
        dtr_enable = 1;
    }
    else
    {
        dtr_enable = 0;
    }
}

#endif

