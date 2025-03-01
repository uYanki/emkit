/*
 * Copyright (c) 2024, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "usbd_core.h"
#include "usbd_cdc_acm.h"

#define USBD_VID           0xFFFF
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

/*!< endpoint address */
#define CDC1_IN_EP  0x81
#define CDC1_OUT_EP 0x01
#define CDC1_INT_EP 0x82

#define CDC2_IN_EP  0x83
#define CDC2_OUT_EP 0x03
#define CDC2_INT_EP 0x84

#define CDC3_IN_EP  0x85
#define CDC3_OUT_EP 0x05
#define CDC3_INT_EP 0x86

/*!< config descriptor size */
#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN + CDC_ACM_DESCRIPTOR_LEN + CDC_ACM_DESCRIPTOR_LEN)

static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0200, 0x01),
};

static const uint8_t config_descriptor_hs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x06, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC1_INT_EP, CDC1_OUT_EP, CDC1_IN_EP, USB_BULK_EP_MPS_HS, 0x02),
    CDC_ACM_DESCRIPTOR_INIT(0x02, CDC2_INT_EP, CDC2_OUT_EP, CDC2_IN_EP, USB_BULK_EP_MPS_HS, 0x02),
    CDC_ACM_DESCRIPTOR_INIT(0x04, CDC3_INT_EP, CDC3_OUT_EP, CDC3_IN_EP, USB_BULK_EP_MPS_HS, 0x02),
};

static const uint8_t config_descriptor_fs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x06, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC1_INT_EP, CDC1_OUT_EP, CDC1_IN_EP, USB_BULK_EP_MPS_FS, 0x02),
    CDC_ACM_DESCRIPTOR_INIT(0x02, CDC2_INT_EP, CDC2_OUT_EP, CDC2_IN_EP, USB_BULK_EP_MPS_FS, 0x02),
    CDC_ACM_DESCRIPTOR_INIT(0x04, CDC3_INT_EP, CDC3_OUT_EP, CDC3_IN_EP, USB_BULK_EP_MPS_FS, 0x02),
};

static const uint8_t device_quality_descriptor[] = {
    USB_DEVICE_QUALIFIER_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, 0x01),
};

static const uint8_t other_speed_config_descriptor_hs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x06, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC1_INT_EP, CDC1_OUT_EP, CDC1_IN_EP, USB_BULK_EP_MPS_HS, 0x02),
    CDC_ACM_DESCRIPTOR_INIT(0x02, CDC2_INT_EP, CDC2_OUT_EP, CDC2_IN_EP, USB_BULK_EP_MPS_HS, 0x02),
    CDC_ACM_DESCRIPTOR_INIT(0x04, CDC3_INT_EP, CDC3_OUT_EP, CDC3_IN_EP, USB_BULK_EP_MPS_HS, 0x02),
};

static const uint8_t other_speed_config_descriptor_fs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x06, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC1_INT_EP, CDC1_OUT_EP, CDC1_IN_EP, USB_BULK_EP_MPS_FS, 0x02),
    CDC_ACM_DESCRIPTOR_INIT(0x02, CDC2_INT_EP, CDC2_OUT_EP, CDC2_IN_EP, USB_BULK_EP_MPS_FS, 0x02),
    CDC_ACM_DESCRIPTOR_INIT(0x04, CDC3_INT_EP, CDC3_OUT_EP, CDC3_IN_EP, USB_BULK_EP_MPS_FS, 0x02),
};

static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "CherryUSB",                  /* Manufacturer */
    "CherryUSB CDC DEMO",         /* Product */
    "2022123456",                 /* Serial Number */
};

static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    return device_descriptor;
}

static const uint8_t *config_descriptor_callback(uint8_t speed)
{
    if (speed == USB_SPEED_HIGH) {
        return config_descriptor_hs;
    } else if (speed == USB_SPEED_FULL) {
        return config_descriptor_fs;
    } else {
        return NULL;
    }
}

static const uint8_t *device_quality_descriptor_callback(uint8_t speed)
{
    return device_quality_descriptor;
}

static const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    if (index >= (sizeof(string_descriptors) / sizeof(char *))) {
        return NULL;
    }
		
    return string_descriptors[index];
}

static const uint8_t *other_speed_config_descriptor_callback(uint8_t speed)
{
    if (speed == USB_SPEED_HIGH) {
        return other_speed_config_descriptor_hs;
    } else if (speed == USB_SPEED_FULL) {
        return other_speed_config_descriptor_fs;
    } else {
        return NULL;
    }
}

const struct usb_descriptor cdc_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
	  .other_speed_descriptor_callback = other_speed_config_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback
};


USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer[2048]; /* 2048 is only for test speed , please use CDC_MAX_MPS for common*/
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t write_buffer[2048];

volatile bool ep_tx_busy_flag = false;

static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    switch (event) {
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
            ep_tx_busy_flag = false;
            /* setup first out ep read transfer */
            usbd_ep_start_read(busid, CDC1_OUT_EP, read_buffer, 2048);
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;

        default:
            break;
    }
}

void usbd_cdc1_acm_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    USB_LOG_RAW("actual out len:%d\r\n", nbytes);

    usbd_ep_start_read(busid, ep, &read_buffer[0], usbd_get_ep_mps(busid, ep));
    usbd_ep_start_write(busid, CDC1_IN_EP, &read_buffer[0], nbytes);
}

void usbd_cdc2_acm_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    USB_LOG_RAW("actual out len:%d\r\n", nbytes);

    usbd_ep_start_read(busid, ep, &read_buffer[0], usbd_get_ep_mps(busid, ep));
    usbd_ep_start_write(busid, CDC2_IN_EP, &read_buffer[0], nbytes);
}

void usbd_cdc3_acm_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    USB_LOG_RAW("actual out len:%d\r\n", nbytes);

    usbd_ep_start_read(busid, ep, &read_buffer[0], usbd_get_ep_mps(busid, ep));
    usbd_ep_start_write(busid, CDC3_IN_EP, &read_buffer[0], nbytes);
}

void usbd_cdc_acm_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    USB_LOG_RAW("actual in len:%d\r\n", nbytes);

    if ((nbytes % usbd_get_ep_mps(busid, ep)) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(busid, ep, NULL, 0);
    } else {
        ep_tx_busy_flag = false;
    }
}

/*!< endpoint call back */
struct usbd_endpoint cdc1_out_ep = {
    .ep_addr = CDC1_OUT_EP,
    .ep_cb = usbd_cdc1_acm_bulk_out
};

struct usbd_endpoint cdc1_in_ep = {
    .ep_addr = CDC1_IN_EP,
    .ep_cb = usbd_cdc_acm_bulk_in
};

struct usbd_endpoint cdc2_out_ep = {
    .ep_addr = CDC2_OUT_EP,
    .ep_cb = usbd_cdc2_acm_bulk_out
};
struct usbd_endpoint cdc2_in_ep = {
    .ep_addr = CDC2_IN_EP,
    .ep_cb = usbd_cdc_acm_bulk_in
};

struct usbd_endpoint cdc3_out_ep = {
    .ep_addr = CDC3_OUT_EP,
    .ep_cb = usbd_cdc3_acm_bulk_out
};

struct usbd_endpoint cdc3_in_ep = {
    .ep_addr = CDC3_IN_EP,
    .ep_cb = usbd_cdc_acm_bulk_in
};


static struct usbd_interface intf0;
static struct usbd_interface intf1;
static struct usbd_interface intf2;
static struct usbd_interface intf3;
static struct usbd_interface intf4;
static struct usbd_interface intf5;
/* function ------------------------------------------------------------------*/

void cdc_acm_init(uint8_t busid, uint32_t reg_base)
{
    usbd_desc_register(busid, &cdc_descriptor);
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf0));
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf1));
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf2));
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf3));
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf4));
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf5));
    usbd_add_endpoint(busid, &cdc1_out_ep);
    usbd_add_endpoint(busid, &cdc1_in_ep);
    usbd_add_endpoint(busid, &cdc2_out_ep);
    usbd_add_endpoint(busid, &cdc2_in_ep);
    usbd_add_endpoint(busid, &cdc3_out_ep);
    usbd_add_endpoint(busid, &cdc3_in_ep);
    usbd_initialize(busid, reg_base, usbd_event_handler);
}

volatile uint8_t dtr_enable = 0;

void usbd_cdc_acm_set_dtr(uint8_t busid, uint8_t intf, bool dtr)
{
    (void)busid;
    (void)intf;

    if (dtr) {
        dtr_enable = 1;
    }
}

void cdc_acm_data_send_with_dtr_test(uint8_t busid)
{
    write_buffer[0] = 'h';
    write_buffer[1] = 'p';
    write_buffer[2] = 'm';
    write_buffer[3] = 'i';
    write_buffer[4] = 'c';
    write_buffer[5] = 'r';
    write_buffer[6] = 'o';
    write_buffer[7] = 0x0D;
    write_buffer[8] = 0x0A;
    
    write_buffer[2046] = 0x0D;
    write_buffer[2047] = 0x0A;

    ep_tx_busy_flag = true;

    memset(&write_buffer[9], 'a', 2037);
    usbd_ep_start_write(busid, CDC1_IN_EP, &write_buffer[0], 2048);

    while (ep_tx_busy_flag) {
    
    }

    ep_tx_busy_flag = true;

    memset(&write_buffer[9], 'b', 2037);
    usbd_ep_start_write(busid, CDC2_IN_EP, &write_buffer[0], 2048);

    while (ep_tx_busy_flag) {
    }

    ep_tx_busy_flag = true;

    memset(&write_buffer[9], 'c', 2037);
    usbd_ep_start_write(busid, CDC3_IN_EP, &write_buffer[0], 2048);

    while (ep_tx_busy_flag) {
    }
}
