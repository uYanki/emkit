#pragma once

#if 0
#include <stdint.h>
#else
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
#endif

#include <windows.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "ch341dll.h"

class ch341 {
private:
    uint8_t m_port;

public:
    enum {
        I2C_20KHZ  = 0x00,    // 低速 0b00000000
        I2C_100KHZ = 0x01,    // 标准 0b00000001
        I2C_400KHZ = 0x02,    // 快速 0b00000010
        I2C_750KHZ = 0x03,    // 高速 0b00000011

        SPI_SINGLE = 0x00,    // D3时钟/D5出/D7入 0b00000000
        SPI_DUAL   = 0x04,    // D3时钟/D5,D4出/D7,D6入 0b00000100

        SPI_LSBFIRST = 0x00,  // 0b00000000
        SPI_MSBFIRST = 0x80,  // 0b10000000
    };

    typedef uint8_t MODE;

    ch341(uint8_t port = 0, MODE mode = SPI_MSBFIRST | SPI_SINGLE | I2C_750KHZ)
    {
        m_port = port;
        assert(!(LoadLibrary("CH341DLL.DLL") == NULL));
        assert(!(CH341OpenDevice(port) == INVALID_HANDLE_VALUE));
        CH341SetStream(port, mode);
    }

    ~ch341() {}

    /**
     * @defgroup i2c
     * {
     */

    bool i2c_tx(uint8_t dev, uint8_t reg, uint8_t val) { return CH341WriteI2C(m_port, dev, reg, val); }

    bool i2c_rx(uint8_t dev, uint8_t reg, uint8_t* val) { return CH341ReadI2C(m_port, dev, reg, val); }

    /**
     * }
     */

    /**
     * @defgroup gpio
     * {
     */

    enum PIN {
        D0 = 0,
        D1 = 1,
        D2 = 2,
        D3 = 3,
        D4 = 4,
        D5 = 5,
    };

    enum DIR {
        INPUT  = 0,
        OUTPUT = 1,
    };

    enum LEVEL {
        LOW  = 0,
        HIGH = 1,
    };

    // CH341Set_D5_D0(0, 0x1F, 0x00);  // d0~d5, output mode , level = low
    bool gpio_cfg(uint8_t dir, uint8_t val) { return CH341Set_D5_D0(m_port, dir, val); }

    bool gpio_set(PIN pin, bool val) { return 0; }

    bool gpio_get(PIN pin, bool val) { return 0; }

    /**
     * }
     */
};
