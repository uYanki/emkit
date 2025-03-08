

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#if 0
#define I2C_PORT    i2c_default               // i2c0
#define I2C_PIN_SDA PICO_DEFAULT_I2C_SDA_PIN  // gpio4
#define I2C_PIN_SCL PICO_DEFAULT_I2C_SCL_PIN  // gpio5
#else
#define I2C_PORT    i2c0
#define I2C_PIN_SDA 16
#define I2C_PIN_SCL 17
#endif

// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx (保留地址)
static inline bool is_reserved_addr(uint8_t addr)
{
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

uint8_t i2cbus_address_scanner(void)
{
    int addr = 0, ret;

    uint8_t step, cnt = 0;
    uint8_t rxdata;

    printf("\ni2cbus 7-bit address scanner: ");
    printf("< sda = %d, scl = %d >", I2C_PIN_SDA, I2C_PIN_SCL);
    printf("\n     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f");

    while (addr < 0x7F) {
        printf("\n%02x: ", addr);

        step = 0x10;
        do {
            if (is_reserved_addr(addr)) {
                // skip it
                ret = PICO_ERROR_GENERIC;
            } else {
                ret = i2c_read_blocking(I2C_PORT, addr, &rxdata, 1, false);
            }

            if (ret < 0) {
                printf("-- ");
            } else {
                printf("%02x ", addr);
                ++cnt;
            }

            ++addr;
            sleep_ms(1);
        } while (--step);
    }

    printf("\n>> %d devices detected in this scan\n", cnt);

    return cnt;
}

int main()
{
    // Enable UART so we can print status output
    stdio_init_all();

    // This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(I2C_PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_PIN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_PIN_SDA);
    gpio_pull_up(I2C_PIN_SCL);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(I2C_PIN_SDA, I2C_PIN_SCL, GPIO_FUNC_I2C));

    while (1) {
        sleep_ms(3000);
        // address scan
        i2cbus_address_scanner();
    }

    return 0;
}
