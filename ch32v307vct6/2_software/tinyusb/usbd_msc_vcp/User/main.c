/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 USART Print debugging routine:
 USART1_Tx(PA9).
 This example demonstrates using USART1(PA9) as a print debug port output.

*/

#include "ch32v30x.h"
#include "tusb.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

void cdc_task(void);

/*------------- MAIN -------------*/

int main(void) {

  board_init();

  printf("SystemClk:%d\r\n", SystemCoreClock);

  // init device stack on configured roothub port
  tusb_rhport_init_t dev_init = {
    .role = TUSB_ROLE_DEVICE,
    .speed = TUSB_SPEED_AUTO
  };
  tusb_init(BOARD_TUD_RHPORT, &dev_init);

  while (1) {
    tud_task(); // tinyusb device task
    cdc_task();
  }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) {
}

// Invoked when device is unmounted
void tud_umount_cb(void) {
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
  (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {
}


//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
void cdc_task(void) {
  // connected() check for DTR bit
  // Most but not all terminal client set this when making connection
  // if ( tud_cdc_connected() )
  {
    // connected and there are data available
    if (tud_cdc_available()) {
      // read data
      char buf[64];
      uint32_t count = tud_cdc_read(buf, sizeof(buf));
      (void) count;

      // Echo back
      // Note: Skip echo by commenting out write() and write_flush()
      // for throughput test e.g
      //    $ dd if=/dev/zero of=/dev/ttyACM0 count=10000
      tud_cdc_write(buf, count);
      tud_cdc_write_flush();
    }
  }
}

// Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
  (void) itf;
  (void) rts;

  // TODO set some indicator
  if (dtr) {
    // Terminal connected
  } else {
    // Terminal disconnected
  }
}

// Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf) {
  (void) itf;
}
