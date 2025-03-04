#ifndef __I2C_H__
#define __I2C_H__

#include "types.h"

//-----------------------------------------------------------------------------------
// Configuration
//-----------------------------------------------------------------------------------

/**
 *                drv (swi2c/hwi2c) ---+
 *                                     +---> i2c_bus ---+
 *  (i2c_msg) xfer -+-----+ i2c_ops ---+                +---> i2c_dev
 *           ioctl -+                           addr ---+
 */

typedef struct i2c_msg i2c_msg_t;
typedef struct i2c_bus i2c_bus_t;
typedef struct i2c_ops i2c_ops_t;
typedef struct i2c_dev i2c_dev_t;

#define I2CMST_ADDR_7BIT   BV(0)  // 7-bits address mode
#define I2CMST_ADDR_10BIT  BV(1)  // 10-bits address mode

#define I2CMST_WRITE       BV(0)  // write data from master to slave
#define I2CMST_RAED        BV(2)  // read data from slave to master

#define I2CMST_NO_START    BV(3)  // dont't generate start singal
#define I2CMST_NO_STOP     BV(4)  // dont't generate stop singal

// under normal circumstances, send ack when the current read length is less
// than the target read length, send nack when the current read length reaches
// the target read length. if this bit is set, no ack and nack signal will be
// sent when reading data.
#define I2CMST_NO_ACK      BV(5)

// generate stop singal when error occurs
#define I2CMST_ERR_STOP    BV(0)  // error trigger stop
#define I2CMST_ERR_SKIP    BV(6)  // error trigger none

// ignore nack singal when write address or data
#define I2CMST_IGNORE_NACK BV(7)

struct i2c_msg {
    uint16_t addr;  // slave address
    uint8_t* dat;
    uint16_t len;
    uint16_t flgs;  // flags: I2C_MASTER_xxx
};

// operations
struct i2c_ops {
    err_t (*xfer)(i2c_bus_t* bus, i2c_msg_t* msgs, uint16_t cnt);
    err_t (*ioctl)(int32_t cmd, void* args);
};

// device
struct i2c_bus {
    void*      drv;
    i2c_ops_t* ops;
};

struct i2c_dev {
    i2c_bus_t* bus;
    uint16_t   addr;
};

//-----------------------------------------------------------------------------------
// Function
//-----------------------------------------------------------------------------------

static inline err_t i2cmst_xfer(i2c_bus_t* bus, i2c_msg_t* msgs, uint16_t cnt) { return bus->ops->xfer(bus, msgs, cnt); }

//

err_t i2cmst_read_bytes(i2c_bus_t* bus, uint16_t addr, uint16_t reg, uint8_t* dat, uint16_t len);
err_t i2cmst_write_bytes(i2c_bus_t* bus, uint16_t addr, uint16_t reg, uint8_t* dat, uint16_t len);

static inline err_t i2cmst_read_byte(i2c_bus_t* bus, uint16_t addr, uint16_t reg, uint8_t* dat) { return i2cmst_read_bytes(bus, addr, reg, dat, 1); }
static inline err_t i2cmst_write_byte(i2c_bus_t* bus, uint16_t addr, uint16_t reg, uint8_t dat) { return i2cmst_write_bytes(bus, addr, reg, &dat, 1); }

//

static inline err_t i2cdev_read_bytes(i2c_dev_t* dev, uint16_t reg, uint8_t* dat, uint16_t len) { return i2cmst_read_bytes(dev->bus, dev->addr, reg, dat, len); }
static inline err_t i2cdev_write_bytes(i2c_dev_t* dev, uint16_t reg, uint8_t* dat, uint16_t len) { return i2cmst_write_bytes(dev->bus, dev->addr, reg, dat, len); }

static inline err_t i2cdev_read_byte(i2c_dev_t* dev, uint16_t reg, uint8_t* dat) { return i2cdev_read_bytes(dev, reg, dat, 1); }
static inline err_t i2cdev_write_byte(i2c_dev_t* dev, uint16_t reg, uint8_t dat) { return i2cdev_write_bytes(dev, reg, &dat, 1); }

err_t i2cdev_read_word(i2c_dev_t* dev, uint16_t reg, uint16_t* dat, endian_e endian);
err_t i2cdev_write_word(i2c_dev_t* dev, uint16_t reg, uint16_t dat, endian_e endian);

err_t i2cdev_read_mask(i2c_dev_t* dev, uint16_t reg, uint8_t msk, uint8_t* dat);
err_t i2cdev_write_mask(i2c_dev_t* dev, uint16_t reg, uint8_t msk, uint8_t dat);

static inline err_t i2cdev_read_bits(i2c_dev_t* dev, uint16_t reg, uint8_t stb, uint8_t len, uint8_t* dat)
{
    uint8_t msk = (~(0xFF << len)) << stb;
    err_t   ret = i2cdev_read_mask(dev, reg, msk, dat);
    if (IS_OK(ret)) { *dat >>= stb; }
    return ret;
}
static inline err_t i2cdev_write_bits(i2c_dev_t* dev, uint16_t reg, uint8_t stb, uint8_t len, uint8_t dat)
{
    uint8_t msk = (~(0xFF << len)) << stb;
    return i2cdev_write_mask(dev, reg, msk, dat << stb);
}

static inline err_t i2cdev_read_bit(i2c_dev_t* dev, uint16_t reg, uint8_t bit, uint8_t* dat) { return i2cdev_read_bits(dev, reg, bit, 1, dat); }
static inline err_t i2cdev_write_bit(i2c_dev_t* dev, uint16_t reg, uint8_t bit, uint8_t dat) { return i2cdev_write_bits(dev, reg, bit, 1, dat ? 1 : 0); }

//

/**
 * @brief device 7-bit address scanner
 * @return number of scanned devices
 */
uint8_t i2cmst_scanner(i2c_bus_t* bus);

/**
 * @brief memory reader
 *
 * @param dev   i2c device pointer
 * @param start register start address
 * @param end   register end address
 * @param fmt   output format (UINT_FORMAT_xxx: 0:bin 1:dec 2:hex)
 *
 * @return status code
 */
err_t i2cdev_viewer(i2c_dev_t* dev, uint8_t start, uint8_t end, uint_fmt_e fmt);

/**
 * @brief memory writer
 *
 * @param dev   i2c device pointer
 * @param pairs register-value pairs
 * @param cnt   count of pairs
 *
 * @return status code
 */
err_t i2cdev_mapper(i2c_dev_t* dev, uint8_t pairs[][2], uint16_t cnt);

#endif
