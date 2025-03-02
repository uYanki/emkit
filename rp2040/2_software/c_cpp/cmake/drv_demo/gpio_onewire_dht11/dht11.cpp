
//                  T1           T2       T3       TS      TB0       TS         TB1
// ____          _______. ___          ________          ________           ____________         __
//     \  18ms  /  24us      \  80us  /  80us  \  48us  /  24us  \  48us   /    72us    \       /
//      --------              ---|----    |     ---|----          ----|----       |      --...--
//     ^        ^       ^     ^  ^   ^    ^     ^  ^   ^          ^   ^   ^       ^      ^     ^
//    (1)      (2)     (3)   (4)(A) (5)  (B)   (6)(C) (7)        (6)(7+dt)(8)   (8+dt)  (6)   (9)
//
// Line has pullup resistor.
// (1) MCU drives line LOW for 18ms
// (2) MCU drives line HIGH for 24us
// (3) MCU releases line waits for DHT11 to drive line LOW
// (4) DHT11 drives line LOW for 80us
// (5) DHT11 drives line HIGH for 80us
// (6) DHT11 drives line LOW for 48us. Marks bit start.
// (7) DHT11 drives line HIGH for 24us. Indicates 0 bit.
// (8) DHT11 drives line HIGH for 72
// (9) DHT11 releases line once 5*8 bits have been transmitted.
//
// Read line dt=48us after (7) or (8). Value is bit value.
// If 1 wait for low

#include "dht11.h"

DHT11::DHT11(uint8_t pin)
{
    m_pin = pin;
    gpio_init(pin);
    sleep_ms(1000);
}

DHT11::~DHT11()
{
    gpio_deinit(m_pin);
}

bool DHT11::read()
{
    uint16_t count = 0;
    uint64_t raw   = 0;

    gpio_set_dir(m_pin, GPIO_OUT);
    gpio_put(m_pin, 0);
    sleep_ms(20);
    gpio_set_dir(m_pin, GPIO_IN);

    while (gpio_get(m_pin) == 1) {
        count++;
        sleep_us(5);
        if (count == POLLING_LIMIT) {
            return false;
        }
    }

    count = 0;
    while (gpio_get(m_pin) == 0) {
        count++;
        sleep_us(5);
        if (count == POLLING_LIMIT) {
            return false;
        }
    }

    count = 0;
    while (gpio_get(m_pin) == 1) {
        count++;
        sleep_us(5);
        if (count == POLLING_LIMIT) {
            return false;
        }
    }

    // transmission start
    for (int i = 0; i < 40; i++) {
        count = 0;
        // ~50us
        while (gpio_get(m_pin) == 0) {
            sleep_us(5);
        }
        // bit 0 or 1
        while (gpio_get(m_pin) == 1) {
            sleep_us(5);
            count++;
        }
        if (count >= THRESHOLD) {
            raw |= 1;
        }
        raw <<= 1;
    }

    uint8_t temp_int = 0xFF & (raw >> 16);
    uint8_t temp_dec = 0xFF & (raw >> 8);
    uint8_t rh_int   = 0xFF & (raw >> 32);
    uint8_t rh_dec   = 0xFF & (raw >> 24);
    uint8_t checksum = 0xFF & (raw >> 0);

    // check if raw data is valid
    if ((temp_int + temp_dec + rh_int + rh_dec) != checksum) {
        return false;
    }

    m_temperature = (int)temp_int + 0.1 * temp_dec;
    m_humidity    = (int)rh_int + 0.1 * rh_dec;

    return true;
}
