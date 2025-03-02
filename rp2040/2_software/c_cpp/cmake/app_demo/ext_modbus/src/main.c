
#include <stdio.h>
#include <stdlib.h>

#include <hardware/irq.h>
#include <hardware/structs/scb.h>
#include <hardware/uart.h>
#include <hardware/watchdog.h>
#include <pico/bootrom.h>
#include <pico/stdlib.h>

#include "bsp/board.h"
#include "usdk.types.h"

#include "tusb.h"

#include "para_table.h"
#include "para_group.h"

#include "mbmst.h"
#include "mbslv.h"

static para_tbl_t m_paratbl;

static mbslv_context_t m_mbslv_ctx;
static mbmst_context_t m_mbmst_ctx;
static uint16_t        system_error = 0;

static void ecb_mbmst_tx(uint8_t* data, size_t size)
{
    gpio_put(MB_DE_PIN, 1);

    uart_write_blocking(MB_UART, data, size);

    // Wait until fifo is drained so we now when to turn off the driver enable pin.
    uart_tx_wait_blocking(MB_UART);

    gpio_put(MB_DE_PIN, 0);
}

static void ecb_mbmst_rx(void)  // Interrupt
{
    mbmst_rx(&m_mbmst_ctx, uart_getc(MB_UART));
}

static uint32_t ecb_mb_get_tick_ms(void)
{
    return time_us_64() / 1000;
}

static void ecb_mbslv_tx(uint8_t* data, size_t size)
{
    if (tud_cdc_n_connected(USB_ITF_MB)) {
        tud_cdc_n_write(USB_ITF_MB, data, size);
        tud_cdc_n_write_flush(USB_ITF_MB);
    }
}

#define ecb_mbslv_rx tud_cdc_rx_cb
void ecb_mbslv_rx(uint8_t i)  // irq
{
    char c;
    if (USB_ITF_MB == i) {
        while (tud_cdc_n_available(i)) {
            if (tud_cdc_n_read(i, &c, i)) {
                mbslv_rx(&m_mbslv_ctx, c);
            }
        }
    }
}

static void ecb_mbmst_status(uint8_t address, uint8_t function, uint8_t error_code)
{
#if CONFIG_EXT_MODULE

    uint8_t i;

    for (i = 0; i < EXT_MODULE_COUNT; ++i) {
        if (g_paragrp.EXT[i].salveID == address) {
            g_paragrp.EXT[i].error |= error_code & 0xFF;
            break;
        }
    }

#endif
}

static void sys_reset(void)
{
    scb_hw->aircr |= M0PLUS_AIRCR_SYSRESETREQ_BITS;
    for (;;) {}
}

static mb_result_e ecb_mbslv_write_single_holding_register(uint16_t reg, uint16_t value)
{
    switch (reg) {
        case MBREG_SLAVE_ID:
            if (value == 0 || value > 255) {
                return MB_ERROR_ILLEGAL_DATA_VALUE;
            }
            g_paragrp.salveID = value;
            return MB_NO_ERROR;
        case MBREG_CMDWORD:
            g_paragrp.cmdword.value = value;
            return MB_NO_ERROR;

#if CONFIG_LED_MODULE
        case MBREG_LED:
            g_paragrp.blink.period = value;
            return MB_NO_ERROR;
#endif

#if CONFIG_PID_MODULE
        case MBREG_PID_START ... MBREG_PID_END:
            ((u16*)&g_paragrp.PID)[reg - MBREG_PID_START] = value;
            return MB_NO_ERROR;
#endif

#if CONFIG_EXT_MODULE
        case MBREG_EXT_START ... MBREG_EXT_END:
            ((u16*)&g_paragrp.EXT)[reg - MBREG_EXT_START] = value;
            return MB_NO_ERROR;
        case MBREG_EXT_MAP_START ... MBREG_EXT_MAP_END:
            reg -= MBREG_EXT_MAP_START;
            mbmst_write_single_register(&m_mbmst_ctx, g_paragrp.EXT[reg / EXT_MODULE_BUFSIZE].salveID, reg % EXT_MODULE_BUFSIZE, value);
            return MB_NO_ERROR;
#endif

        default:
            return MB_ERROR_ILLEGAL_DATA_ADDRESS;
    }
}

static mb_result_e ecb_mbslv_write_holding_registers(uint16_t start, uint16_t* data, uint16_t count)
{
    mb_result_e res;

    for (int i = 0; i < count; i++) {
        res = ecb_mbslv_write_single_holding_register(start + i, data[i]);
        if (res != MB_NO_ERROR) {
            return res;
        }
    }

    return MB_NO_ERROR;
}

static mb_result_e ecb_mbslv_read_single_holding_register(uint16_t reg, uint16_t* value)
{
    switch (reg) {
        case MBREG_SLAVE_ID:
            *value = g_paragrp.salveID;
            return MB_NO_ERROR;
        case MBREG_CMDWORD:
            *value = g_paragrp.cmdword.value;
            return MB_NO_ERROR;

#if CONFIG_LED_MODULE
        case MBREG_LED:
            *value = g_paragrp.blink.period;
            return MB_NO_ERROR;
#endif

#if CONFIG_PID_MODULE
        case MBREG_PID_START ... MBREG_PID_END:
            *value = ((u16*)&g_paragrp.PID)[reg - MBREG_PID_START];
            return MB_NO_ERROR;
#endif

#if CONFIG_EXT_MODULE
        case MBREG_EXT_START ... MBREG_EXT_END:
            *value = ((u16*)&g_paragrp.EXT)[reg - MBREG_EXT_START];
            return MB_NO_ERROR;
        case MBREG_EXT_MAP_START ... MBREG_EXT_MAP_END:
            *value = ((u16*)g_extgrp)[reg - MBREG_EXT_MAP_START];
            return MB_NO_ERROR;
#endif
        default:
            return MB_ERROR_ILLEGAL_DATA_ADDRESS;
    }
}

static mb_result_e ecb_mbslv_read_holding_registers(uint16_t start, uint16_t count)
{
    uint16_t val;
    for (int i = 0; i < count; i++) {
        if (ecb_mbslv_read_single_holding_register(start + i, &val) == MB_NO_ERROR) {
            mbslv_add_response(&m_mbslv_ctx, val);
        } else {
            return MB_ERROR_ILLEGAL_DATA_ADDRESS;
        }
    }
    return MB_NO_ERROR;
}

static void mbmst_tx_request(uint8_t* data, size_t size)
{
    mbmst_send_raw(&m_mbmst_ctx, data, size);
}

void pid_task(void)
{
#if CONFIG_PID_MODULE

    static PID_t pid = {
        .Kp    = &(g_paragrp.PID.Kp),
        .Ki    = &(g_paragrp.PID.Ki),
        .Kd    = &(g_paragrp.PID.Kd),
        .ref   = &(g_paragrp.PID.ref),
        .fbk   = &(g_paragrp.PID.fbk),
        .ramp  = &(g_paragrp.PID.ramp),
        .lower = &(g_paragrp.PID.lower),
        .upper = &(g_paragrp.PID.upper),
        .Ts    = 1,
        .cb    = &PID_Handler_Basic,
    };

    static absolute_time_t tmr = 0;

    if (absolute_time_diff_us(tmr, get_absolute_time()) > 0) {
        tmr               = make_timeout_time_ms(g_paragrp.PID.Ts * 1000);
        g_paragrp.PID.fbk = g_paragrp.PID.out;
        g_paragrp.PID.out = pid.cb(&pid);  // PID process
    }

#endif
}

void led_task(void)
{
#if CONFIG_LED_MODULE

    static bool            on  = false;
    static absolute_time_t tmr = 0;

    if (g_paragrp.blink.period) {
        if (absolute_time_diff_us(tmr, get_absolute_time()) > 0) {
            tmr = make_timeout_time_ms(g_paragrp.blink.period);
            (on = !on) ? board_led_on() : board_led_off();
            printf("led tgl\n");
        }
    }

#endif
}

void cmdword_task()
{
    if (g_paragrp.cmdword.paratbl_reload) {
        paratbl_load(&m_paratbl);
    }
    if (g_paragrp.cmdword.paratbl_save) {
        paratbl_save(&m_paratbl);
    }
    if (g_paragrp.cmdword.paratbl_reset) {
        paragrp_reset();
    }
    // sys rst at last
    if (g_paragrp.cmdword.system_reset) {
        sys_reset();
    }
    g_paragrp.cmdword.value = 0;  // clear
}

void ecb_mbmst_read_holding_registers(uint8_t address, uint16_t start, uint16_t count, uint16_t* data)
{
#if CONFIG_EXT_MODULE

    uint8_t i;

    // [n][regStart, regStart+regCount] -> [n][0,regCount]

    for (i = 0; i < EXT_MODULE_COUNT; ++i) {
        if (g_paragrp.EXT[i].salveID == address) {
            uint16_t* reg = (uint16_t*)g_extgrp;
            reg += i * EXT_MODULE_BUFSIZE;
            reg += start - g_paragrp.EXT[i].regStart;
            while (count--) {
                *reg++ = *data++;
            }
            break;
        }
    }

#endif
}

void ext_task()
{
#if CONFIG_EXT_MODULE

    // 内存映射

    static absolute_time_t tmr[EXT_MODULE_COUNT] = {0};

    uint8_t i;

    for (i = 0; i < EXT_MODULE_COUNT; ++i) {
        if (g_paragrp.EXT[i].salveID > 0 &&
            g_paragrp.EXT[i].regCount > 0 &&
            g_paragrp.EXT[i].scanPeriod > 0) {
            if (absolute_time_diff_us(tmr[i], get_absolute_time()) > 0) {
                tmr[i] = make_timeout_time_ms(g_paragrp.EXT[i].scanPeriod);
                if (g_paragrp.EXT[i].regCount > EXT_MODULE_BUFSIZE) {
                    g_paragrp.EXT[i].regCount = EXT_MODULE_BUFSIZE;
                }
                mbmst_read_holding_registers(&m_mbmst_ctx,
                                             g_paragrp.EXT[i].salveID,
                                             g_paragrp.EXT[i].regStart,
                                             g_paragrp.EXT[i].regCount);
            }
        }
    }

#endif
}

static void setup_uarts(void)
{
    gpio_init(MB_DE_PIN);
    gpio_set_dir(MB_DE_PIN, GPIO_OUT);
    gpio_put(MB_DE_PIN, 0);

    uart_init(MB_UART, MB_UART_BAUD);

    gpio_set_function(MB_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(MB_RX_PIN, GPIO_FUNC_UART);

    gpio_pull_down(MB_RX_PIN);

    uart_set_fifo_enabled(MB_UART, false);

    irq_set_exclusive_handler(MB_UART_IRQ, ecb_mbmst_rx);
    irq_set_enabled(MB_UART_IRQ, true);

    uart_set_irq_enables(MB_UART, true, false);
}

int main(void)
{
    board_init();
    tud_init(TUD_OPT_RHPORT);
    stdio_usb_init();

    // gpio_init(LED_PIN);
    // gpio_set_dir(LED_PIN, GPIO_OUT);

    // 启用看门狗后, 需注意 sleep 延时时间
    watchdog_enable(100, 1);

    setup_uarts();

    paratbl_config(&m_paratbl, &g_paragrp, 0, offsetof(paragrp_t, cmdword));
    paratbl_load(&m_paratbl);
    g_paragrp.cmdword.value = 0;
    if (g_paragrp.salveID == 0) {
        paragrp_reset();
        paratbl_save(&m_paratbl);
    }

    // mb.rx is completed on interrupt
    mbslv_cb_t mbslv = {
        .get_tick_ms              = ecb_mb_get_tick_ms,
        .write_single_register    = ecb_mbslv_write_single_holding_register,
        .read_holding_registers   = ecb_mbslv_read_holding_registers,
        .write_multiple_registers = ecb_mbslv_write_holding_registers,
        .tx                       = ecb_mbslv_tx,  // cdc tx
#if !CONFIG_EXT_MODULE
        // if current frame is not belong mbslv, mbslv will transfer current frame through raw_tx
        .raw_tx = mbmst_tx_request,
#endif
    };
    mbslv_init(&m_mbslv_ctx, g_paragrp.salveID, &mbslv);

    mbmst_cb_t mbmst = {
        .get_tick_ms = ecb_mb_get_tick_ms,
        .status      = ecb_mbmst_status,
        .tx          = ecb_mbmst_tx,
#if !CONFIG_EXT_MODULE
        .raw_tx = ecb_mbslv_tx,
#else
        .read_holding_registers = ecb_mbmst_read_holding_registers,
#endif
    };
    mbmst_init(&m_mbmst_ctx, &mbmst);

    printf("modbus salve id = %d", g_paragrp.salveID);

    for (;;) {
        // virtual serial port (usb)
        tud_task();

        // modbus
        mbslv_task(&m_mbslv_ctx);
        mbmst_task(&m_mbmst_ctx);

        // add tasks at here
        led_task();
        pid_task();
        cmdword_task();
        ext_task();

        // feed wdg
        watchdog_update();
    }
}
