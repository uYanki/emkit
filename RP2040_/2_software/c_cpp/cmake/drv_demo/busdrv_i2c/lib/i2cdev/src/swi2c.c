#include "swi2c.h"

// start singal: sda changes from high to low when scl is high
// stop  singal: sda changes from low to high when scl is high
// data: write SDA when SCL is low and maintain SDA when SCL is high

// 当 SCL 为高电平时, SDA 的电平发生变化, 相当于产生起始信号或终止信号。
// 当 SCL 为低电平时, 主机往 SDA 写数据; 当 SCL 为高电平时, 从机从 SDA 读取。

// 已使用逻辑分析仪测试: ① SCL 翻转间隔相等; ② 从机不应答时, 不丢失应答位...

//-----------------------------------------------------------------------------------
// Port
//-----------------------------------------------------------------------------------

// clang-format off
#define CFG_SDA(dir) if (drv->cfg_scl != nullptr) { drv->cfg_sda(drv->port, dir); }
#define CFG_SCL(dir) if (drv->cfg_scl != nullptr) { drv->cfg_scl(drv->port, dir); }
// clang-format on

#define SET_SDA(val) drv->set_sda(drv->port, val)
#define SET_SCL(val) drv->set_scl(drv->port, val)
#define GET_SDA()    drv->get_sda(drv->port)
#define GET_SCL()    drv->get_scl(drv->port)

#define Delay()      drv->delay(drv->interval)

#define SDA_IN()     CFG_SDA(IN)
#define SDA_OUT()    CFG_SDA(OUT)
#define SCL_IN()     CFG_SCL(IN)
#define SCL_OUT()    CFG_SCL(OUT)

#define SDA_L()      SET_SDA(LOW)
#define SDA_H()      SET_SDA(HIGH)
#define SCL_L()      SET_SCL(LOW)
#define SCL_H()      _SCL_H(drv)

static err_t _SCL_H(swi2c_drv_t* drv)
{
    i2c_time_t t = 0;

    SET_SCL(HIGH);

    if (drv->get_scl != nullptr) {
        SCL_IN();
        // scl pulled down by slave
        //  ( slave is drvy )
        while (GET_SCL() == LOW) {
            t += drv->interval;
            if (t > drv->timeout) {
                SCL_OUT();
                return ERR_BUSY;
            }
        }
        SCL_OUT();
    }

    return SUCCESS;
}

//-----------------------------------------------------------------------------------
// Driver
//-----------------------------------------------------------------------------------

typedef enum {
    ACK  = 0,  // sda = 0
    NACK = 1,  // sda = 1
} swi2c_ack_t;

static void swi2c_start(swi2c_drv_t* drv, bool restart)
{
    if (!restart) {
        // check if the bus is busy

        if (drv->get_scl != nullptr) {
            SCL_IN();  // set scl as input mode
            if (GET_SCL() == LOW) {
                // error: scl is low now
            }
        }

        if (drv->get_sda != nullptr) {
            SDA_IN();  // set sda as input mode
            if (GET_SDA() == LOW) {
                // error: sda is low now
            }
        }
    }

    // set sda as output mode
    SDA_OUT();
    // set scl as output mode
    SCL_OUT();

    if (restart) {
        Delay();
        SDA_H();  // sda = 1
        SCL_H();  // scl = 1
        Delay();
    }

    SDA_L();  // sda: 1 -> 0
    Delay();
    SCL_L();  // scl: 1 -> 0
}

static void swi2c_stop(swi2c_drv_t* drv)
{
    SCL_L();  // scl = 0
    SDA_L();  // sda = 0
    Delay();
    SCL_H();  // scl = 1
    Delay();
    SDA_H();  // sda: 0 -> 1
    Delay();
}

static err_t swi2c_write_byte(swi2c_drv_t* drv, __IN uint8_t dat, __OUT swi2c_ack_t* ack)
{
    uint8_t msk = 0x80;
    err_t   ret;

    /* write byte */

    do {
        SCL_L();
        // sda = x
        if (dat & msk) {
            SDA_H();
        } else {
            SDA_L();
        }

        Delay();

        // scl = 1
        ret = SCL_H();
        Delay();
        if (IS_ERR(ret)) {
            return ret;
        }

    } while (msk >>= 1);

    /* recv ack or nack */

    // scl = 0
    SCL_L();
    // sda = 1
    SDA_H();

    // set sda as input mode
    SDA_IN();
    Delay();

    // scl = 1
    ret = SCL_H();
    Delay();
    if (IS_OK(ret)) {
        // read sda
        switch (GET_SDA()) {
            case LOW: {
                *ack = ACK;
                break;
            }
            default:
            case HIGH: {
                *ack = NACK;
                break;
            }
        }
        ret = SUCCESS;
    }
    // scl = 0
    SCL_L();

    // set sda as output mode
    SDA_OUT();

    return ret;
}

static err_t swi2c_read_byte(swi2c_drv_t* drv, __OUT uint8_t* dat)
{
    uint8_t msk = 0x80;
    err_t   ret;

    // reset buff
    *dat = 0x00;

    // release sda line
    SDA_H();

    // set sda as input mode
    SDA_IN();

    do {
        // scl = 0
        SCL_L();
        Delay();
        // scl = 1
        ret = SCL_H();
        Delay();
        if (IS_ERR(ret)) {
            goto exit;
        }
        // read sda
        if (GET_SDA() == HIGH) {
            *dat |= msk;
        }
    } while (msk >>= 1);

    ret = SUCCESS;

exit:
    // set sda as output mode
    SDA_OUT();

    return ret;
}

static err_t swi2c_send_ack_or_nack(swi2c_drv_t* drv, __IN swi2c_ack_t ack)
{
    err_t ret;

    // scl = 0
    SCL_L();
    switch (ack) {
        default:
        case NACK:
            // sda = 1
            SDA_H();
            break;
        case ACK:
            // sda = 0
            SDA_L();
            break;
    }
    Delay();

    // scl = 1
    ret = SCL_H();
    Delay();
    if (IS_OK(ret)) {
        ret = SUCCESS;
    }

    // scl = 0
    SCL_L();

    return ret;
}

static err_t swi2c_master_xfer(i2c_bus_t* bus, __IN i2c_msg_t* msgs, __IN uint16_t cnt)
{
    swi2c_drv_t* drv;
    i2c_msg_t*   msg;

    uint16_t i;

    uint8_t*    dat;
    uint8_t     len;
    uint16_t    flgs;
    swi2c_ack_t ack;

    err_t ret;

    drv = (swi2c_drv_t*)(bus->drv);

    for (i = 0; i < cnt; ++i) {
        msg  = &msgs[i];
        flgs = msg->flgs;

        //------------------------------------------------------------------------------------------------//
        // generate start singal
        //
        if (!(flgs & I2CMST_NO_START)) {
            uint8_t addr, subaddr;
            uint8_t retries = 5;  // retry

            swi2c_start(drv, (i != 0) ? true : false);

            // send address

            if (flgs & I2CMST_ADDR_10BIT) {
                // 10-bit addr
                addr    = ((msg->addr >> 7) & 0x06) | 0xf0;
                subaddr = msg->addr & 0xff;
            } else {
                // 7-bit addr
                addr = (msg->addr << 1) & 0xFF;
                if (flgs & I2CMST_RAED) { addr |= 1; }
            }

            // sending first addr
            while (retries--) {
                ret = swi2c_write_byte(drv, addr, &ack);
                if (IS_OK(ret)) {
                    // check ack
                    if ((ack == ACK) || (flgs & I2CMST_IGNORE_NACK)) {
                        break;  // ack
                    } else {
                        ret = ERR_TIMEOUT;  // nack
                    }
                }

                // retry
                if (retries) {
                    swi2c_stop(drv);
                    Delay();
                    swi2c_start(drv, true);
                } else {
                    goto error;
                }
            }

            if (flgs & I2CMST_ADDR_10BIT) {
                // sending second addr
                while (0) {
                    ret = swi2c_write_byte(drv, subaddr, &ack);
                    if (IS_OK(ret)) {
                        // check ack
                        if ((ack == ACK) || (flgs & I2CMST_IGNORE_NACK)) {
                            break;  // ack
                        }
                        ret = ERR_TIMEOUT;  // nack
                    }
                    goto error;
                }

                if (flgs & I2CMST_RAED) {
                    addr |= 1;
                }
                // repeated start condition
                while (0) {
                    ret = swi2c_write_byte(drv, addr, &ack);
                    if (IS_OK(ret)) {
                        // check ack
                        if ((ack == ACK) || (flgs & I2CMST_IGNORE_NACK)) {
                            break;  // ack
                        }
                        ret = ERR_TIMEOUT;  // nack
                    }
                    goto error;
                }
            }
        }

        //------------------------------------------------------------------------------------------------//
        // xfer data
        //
        dat = msg->dat;
        len = msg->len;
        // check action
        if (flgs & I2CMST_RAED) {
            // read bytes
            while (len--) {
                // read data
                ret = swi2c_read_byte(drv, dat++);
                if (IS_OK(ret)) {
                    // skip ack
                    if (flgs & I2CMST_NO_ACK) { continue; }
                    // send ack
                    ret = swi2c_send_ack_or_nack(drv, (len == 0) ? NACK : ACK);
                    if (IS_OK(ret)) { continue; }
                }
                goto error;
            }
        } else {
            // write bytes
            while (len--) {
                // write data and read ack
                ret = swi2c_write_byte(drv, *dat++, &ack);
                if (IS_OK(ret)) {
                    if ((ack == ACK) || (flgs & I2CMST_IGNORE_NACK)) {
                        continue;
                    } else {
                        ret = ERR_TIMEOUT;
                    }
                }
                goto error;
            }
        }

        //------------------------------------------------------------------------------------------------//
        // generate stop singal
        //
        if (!(flgs & I2CMST_NO_STOP)) {
            swi2c_stop(drv);
        }
    }

    return SUCCESS;

error:

    if (!(flgs & I2CMST_ERR_SKIP)) {
        swi2c_stop(drv);
    }

    return ret;
}

//-----------------------------------------------------------------------------------
// Interface
//-----------------------------------------------------------------------------------

i2c_ops_t g_swi2c_ops = {
    .xfer  = swi2c_master_xfer,
    .ioctl = nullptr,
};