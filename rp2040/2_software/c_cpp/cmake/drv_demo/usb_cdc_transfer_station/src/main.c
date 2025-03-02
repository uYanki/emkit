#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "tusb.h"

#include "pico/bootrom.h"
#include "pico/multicore.h"
#include "pico/time.h"
#include "pico/util/queue.h"

#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/uart.h"

#define DEVICE_UART_PORT (uart0)
#define DEVICE_UART_IRQ  (UART0_IRQ)
#define DEVICE_UART_BAUD (115200)
#define DEVICE_GPIO_TX   (0)
#define DEVICE_GPIO_RX   (1)

#define LED_PIN          (25)
#define BUF_MAX          (CFG_TUD_CDC_TX_BUFSIZE)

//--------------------------------------------------------------------+

struct commands_t {
    char* command;
    char* description;
    bool (*f_ptr)(void* data);
};

//--------------------------------------------------------------------+
// static function prototypes
//--------------------------------------------------------------------+

static void local_led_init(void);
static void local_uart_init(void);
static void cdc_task(void);
static bool print(const char* fmt, ...);
static void uart_irq(void);
static bool check_commands(uint8_t* cmd);

static bool cb_led_on(void* data);
static bool cb_led_off(void* data);
static bool cb_show_help(void* data);
static bool cb_say_hello(void* data);

//--------------------------------------------------------------------+

static struct commands_t cmds[] = {
    {"led 1", "LED on",    &cb_led_on   },
    {"led 0", "LED off",   &cb_led_off  },
    {"?",     "Show help", &cb_show_help},
};

//--------------------------------------------------------------------+

static void local_led_init(void)
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
}

void core1_task(void)
{
    while (!tusb_inited())
        ;

    local_uart_init();
    local_led_init();

    while (1) {
        cdc_task();
    }
}

int main(void)
{
    tusb_init();
    while (!tusb_inited()) {}

    multicore_launch_core1(core1_task);

    while (1) {
        tud_task();
    }
}

static void uart_irq(void)
{
    static uint8_t count = 0;

    if (uart_is_readable(DEVICE_UART_PORT)) {
        uint8_t ch = uart_getc(DEVICE_UART_PORT);
        print("%c", ch);
    }
}

void local_uart_init(void)
{
    uart_init(DEVICE_UART_PORT, DEVICE_UART_BAUD);
    gpio_set_function(DEVICE_GPIO_TX, GPIO_FUNC_UART);
    gpio_set_function(DEVICE_GPIO_RX, GPIO_FUNC_UART);
    irq_set_exclusive_handler(DEVICE_UART_IRQ, uart_irq);
    irq_set_enabled(DEVICE_UART_IRQ, true);
    uart_set_irq_enables(DEVICE_UART_PORT, true, false);
}

static bool check_commands(uint8_t* cmd)
{
    for (uint8_t i = 0; i < TU_ARRAY_SIZE(cmds); i++) {
        if (strcmp(cmd, cmds[i].command) == 0) {
            cmds[i].f_ptr(NULL);
            return true;
        }
    }

    return false;
}

static bool print(const char* fmt, ...)
{
    unsigned count = 0;
    char     ascii[BUF_MAX];
    va_list  va;

    va_start(va, fmt);
    vsnprintf(ascii, BUF_MAX, fmt, va);
    va_end(va);

    tud_cdc_write(ascii, strlen(ascii));
    tud_cdc_write_flush();
}

static bool cb_led_on(void* data)
{
    gpio_put(LED_PIN, true);
}

static bool cb_led_off(void* data)
{
    gpio_put(LED_PIN, false);
}

static bool cb_say_hello(void* data)
{
    uart_puts(DEVICE_UART_PORT, "hello\r\n");
}

static bool cb_show_help(void* data)
{
    /*
     * split usage into chunks because CDCACM driver supports 64 bytes max
     */

    for (uint8_t i = 0; i < TU_ARRAY_SIZE(cmds); i++) {
        print(cmds[i].command);
        print(" - ");
        print(cmds[i].description);
        print("\r\n");
    }
}

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+

static void cdc_task(void)
{
    static uint8_t  buff[64];
    static uint8_t* p_dest = buff;

    if (tud_cdc_available()) {
        char    dat[64];
        uint8_t cnt = tud_cdc_read(dat, sizeof(dat));

#if 1  // echo
        tud_cdc_write(dat, cnt);
        tud_cdc_write_flush();
#endif

        uint8_t* p_src = (uint8_t*)dat;

        while (cnt--) {
            if (*p_src == '\r') {
                ++p_src;
                continue;
            }

            switch (*p_src) {
                case '\r':  // skip
                    break;
                default:  // copy
                    *p_dest++ = *p_src++;
                    break;
                case '\n':  // doit
                    *p_dest = '\0';
                    if (!check_commands(buff)) {
                        uart_puts(DEVICE_UART_PORT, buff);
                    }
                    p_dest = buff;
                    break;
            }
        }
    }
}

// Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf)
{
    (void)itf;
}
