/**
 * https://github.com/perrochiba/PIO-UART-RP2040
 *
 * TX pin: GPIO2, RX pin: GPIO3
 * RX characters output through USB
 *
 */

#include "uart_pio.h"

#define BAUD_RATE  9600
#define PIO_TX_PIN 2
#define PIO_RX_PIN 3
#define PIO_RX_IRQ PIO0_IRQ_0  // values for pio0: PIO0_IRQ_0, PIO0_IRQ_1. values for pio1: PIO1_IRQ_0, PIO1_IRQ_1

// RX interrupt handler for PIO UART
static void rx_handler(uint8_t data)
{
    printf("%c", data);
}

int main()
{
    stdio_init_all();

    getchar();

    printf("TX GPIO: %d, RX GPIO: %d\n\n", PIO_TX_PIN, PIO_RX_PIN);

    // Definitions for PIO State Machines
    PIO  pio   = pio0;
    uint sm_rx = pio_claim_unused_sm(pio, true);
    uint sm_tx = pio_claim_unused_sm(pio, true);

    // PIO State machines initialization
    uart_tx_init(pio, sm_tx, PIO_TX_PIN, BAUD_RATE);
    uart_rx_init(pio, sm_rx, PIO_RX_PIN, BAUD_RATE, PIO_RX_IRQ);

    // Set RX handler function
    uart_rx_set_handler(rx_handler);

    while (true) {
        printf(".");
        uart_tx_puts(pio, sm_tx, "Hello PIO UART\n");
        sleep_ms(500);
    }
}
