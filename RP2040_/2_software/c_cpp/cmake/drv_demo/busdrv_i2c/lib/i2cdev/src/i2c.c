#include "i2c.h"

uint8_t i2cmst_scanner(i2c_bus_t* bus)
{
    uint8_t cnt = 0;
    uint8_t step;

    i2c_msg_t msg;

    msg.addr = 0;
    msg.flgs = I2CMST_RAED | I2CMST_ADDR_7BIT;
#if 1
    // pico hwi2c
    uint8_t dummy = 0x00;
    msg.dat       = &dummy,
    msg.len       = 1;
#else
    // swi2c ok
    msg.dat = nullptr,
    msg.len = 0;
#endif

    println("i2cdev 7-bit address detector:");
    println("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f");

    while (msg.addr < 0x80) {
        printf("%02x: ", msg.addr);

        step = 0x10;
        do {
            if (IS_OK(i2cmst_xfer(bus, &msg, 1))) {
                printf("%02x ", msg.addr);
                ++cnt;
            } else {
                printf("-- ");
            }
            ++msg.addr;
        } while (--step);

        println("");
    }

    println(">> %d devices detected in this scan", cnt);

    return cnt;
}

err_t i2cdev_mapper(i2c_dev_t* dev, uint8_t pairs[][2], uint16_t cnt)
{
    err_t ret;

    uint16_t idx = 0;

    while (idx < cnt) {
        ret = i2cdev_write_byte(dev, pairs[idx][0], pairs[idx][1]);
        if (IS_ERR(ret)) {
            return ret;
        }
    }

    return SUCCESS;
}

err_t i2cdev_viewer(i2c_dev_t* dev, uint8_t start, uint8_t end, uint_fmt_e fmt)
{
    // 由于某些设备不支持连读, 因此不使用 i2cdev_read_bytes

    uint8_t  dat, mask;
    uint16_t reg, len;

    if (start > end) {
        // swap_int
        start ^= end ^= start ^= end;
    }

    reg = start;
    len = end - start;

    println("i2cdev (0x%02X) - memory viewer ( from 0x%04X to 0x%04X ):", dev->addr, start, end);

    do {
        // address
        printf("[0x%04X] ", reg);
        // read
        if (i2cdev_read_byte(dev, reg, &dat)) {
            // value
            switch (fmt) {
                default:
                case UINT_FORMAT_BIN: {
                    for (mask = 0x80; mask > 0; mask >>= 1) {
                        printf("%c", dat & mask ? '1' : '0');
                    }
                    break;
                }
                case UINT_FORMAT_DEC: {
                    printf("%d", dat);
                    break;
                }
                case UINT_FORMAT_HEX: {
                    printf("0x%02X", dat);
                    break;
                }
            }
            // newline
            println("");
        } else {
            println("fail to read");
            return false;
        }

        ++reg;
    } while (len--);

    return true;
}

err_t i2cmst_read_bytes(i2c_bus_t* bus, uint16_t addr, uint16_t reg, uint8_t* dat, uint16_t len)
{
    i2c_msg_t msg[2];

    msg[0].addr = addr;
    msg[0].flgs = I2CMST_WRITE | I2CMST_NO_STOP;
    msg[0].dat  = (uint8_t*)&reg;
    msg[0].len  = (reg <= 0xFF) ? 1 : 2;

    msg[1].addr = addr;
    msg[1].flgs = I2CMST_RAED;
    msg[1].dat  = dat;
    msg[1].len  = len;

    if (addr < 0x80) {
        msg[0].flgs |= I2CMST_ADDR_7BIT;
        msg[1].flgs |= I2CMST_ADDR_7BIT;
    } else {
        msg[0].flgs |= I2CMST_ADDR_10BIT;
        msg[1].flgs |= I2CMST_ADDR_10BIT;
    }

    return i2cmst_xfer(bus, msg, 2);
}

err_t i2cmst_write_bytes(i2c_bus_t* bus, uint16_t addr, uint16_t reg, uint8_t* dat, uint16_t len)
{
    i2c_msg_t msg[2];

    msg[0].addr = addr;
    msg[0].flgs = I2CMST_WRITE | I2CMST_NO_STOP;
    msg[0].dat  = (uint8_t*)&reg;
    msg[0].len  = (reg <= 0xFF) ? 1 : 2;

    msg[1].addr = addr;
    msg[1].flgs = I2CMST_WRITE | I2CMST_NO_START;
    msg[1].dat  = dat;
    msg[1].len  = len;

    if (addr < 0x80) {
        msg[0].flgs |= I2CMST_ADDR_7BIT;
        msg[1].flgs |= I2CMST_ADDR_7BIT;
    } else {
        msg[0].flgs |= I2CMST_ADDR_10BIT;
        msg[1].flgs |= I2CMST_ADDR_10BIT;
    }

    return i2cmst_xfer(bus, msg, 2);
}

err_t i2cdev_read_word(i2c_dev_t* dev, uint16_t reg, uint16_t* dat, endian_e endian)
{
    uint8_t buf[2];

    err_t ret = i2cdev_write_bytes(dev, reg, buf, 2);

    if (IS_OK(ret)) {
        switch (endian) {
            case BYTE_ORDER_BIG_ENDIAN: {
                *dat = (buf[0] << 8) | buf[1];
                break;
            }
            default:
            case BYTE_ORDER_LITTLE_ENDIAN: {
                *dat = (buf[1] << 8) | buf[0];
                break;
            }
        }
    }

    return ret;
}

err_t i2cdev_write_word(i2c_dev_t* dev, uint16_t reg, uint16_t dat, endian_e endian)
{
    if (endian != BIG_ENDIAN) {
        dat = (dat << 8) | (dat >> 8);
    }

    return i2cdev_write_bytes(dev, reg, (uint8_t*)&dat, 2);
}

err_t i2cdev_read_mask(i2c_dev_t* dev, uint16_t reg, uint8_t msk, uint8_t* dat)
{
    uint8_t buf;

    err_t ret = i2cdev_read_byte(dev, reg, &buf);

    if (IS_OK(ret)) {
        *dat = buf & msk;
    }

    return ret;
}

err_t i2cdev_write_mask(i2c_dev_t* dev, uint16_t reg, uint8_t msk, uint8_t dat)
{
    uint8_t buf;

    err_t ret = i2cdev_read_byte(dev, reg, &buf);

    if (IS_OK(ret)) {
        dat &= msk;
        buf &= ~msk;
        buf |= dat;
        ret = i2cdev_write_byte(dev, reg, buf);
    }

    return ret;
}
