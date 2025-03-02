#include <stdio.h>
#include <string.h>

#include "can2040.h"
#include "RP2040.h"
#include <pico/stdlib.h>
#include <hardware/sync.h>

#define println(fmt, ...) printf(fmt "\r\n", ##__VA_ARGS__)

#define LED_PIN           25

#define CAN_GPIO_RX       10
#define CAN_GPIO_TX       11

static struct can2040 cbus;

static void can2040_cb(struct can2040* cd, uint32_t notify, struct can2040_msg* msg)
{
    switch (notify) {
        case CAN2040_NOTIFY_RX:
            printf("[rx]");
            break;
        case CAN2040_NOTIFY_TX:
            printf("[tx]");
            break;
        case CAN2040_NOTIFY_ERROR:
            printf("[err]");
            return;
    }

    println("id: 0x%x", msg->id);
    println("data (len = %d): 0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x", msg->dlc,
            msg->data[0], msg->data[1], msg->data[2], msg->data[3], msg->data[4], msg->data[5], msg->data[6], msg->data[7]);

    println("--------------------------");
}

static void PIOx_IRQHandler(void)
{
    can2040_pio_irq_handler(&cbus);
}

static void can_setup(void)
{
    uint32_t pio_num   = 0;
    uint32_t sys_clock = 125E6;
    // uint32_t bitrate = 250E3;
    uint32_t bitrate   = 1E6;
    uint32_t gpio_rx   = CAN_GPIO_RX;
    uint32_t gpio_tx   = CAN_GPIO_TX;

    // Setup canbus
    can2040_setup(&cbus, pio_num);
    can2040_callback_config(&cbus, can2040_cb);

    // Enable irqs
    irq_set_exclusive_handler(PIO0_IRQ_0_IRQn, PIOx_IRQHandler);
    NVIC_SetPriority(PIO0_IRQ_0_IRQn, 1);
    NVIC_EnableIRQ(PIO0_IRQ_0_IRQn);

    // Start canbus
    can2040_start(&cbus, sys_clock, bitrate, gpio_rx, gpio_tx);
}

static void can_transmit(void)
{
    struct can2040_msg msg = {
        .id   = 0x18FFA0F9 | CAN2040_ID_EFF,
        .dlc  = 8,
        .data = "can2040",
    };

    int ret = can2040_transmit(&cbus, &msg);

    println("%s to transmit", ret == 0 ? "success" : "fail");
}

static void led_setup(void)
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
}

static void led_blink(void)
{
    gpio_put(LED_PIN, true);
    sleep_ms(500);
    gpio_put(LED_PIN, false);
    sleep_ms(500);
}

int main()
{
    stdio_init_all();

    led_setup();
    can_setup();

    while (true) {
        led_blink();
        can_transmit();
    }
}
