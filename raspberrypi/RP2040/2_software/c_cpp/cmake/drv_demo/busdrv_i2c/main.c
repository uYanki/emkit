

#include "hardware/gpio.h"

#include "swi2c.h"
#include "hwi2c.h"

static void swi2c_set_sda(void* port, pin_lvl_e level)
{
    swi2c_cfg_t* cfg = (swi2c_cfg_t*)port;
    gpio_put(cfg->sda, level == LOW ? 0 : 1);
}

static void swi2c_set_scl(void* port, pin_lvl_e level)
{
    swi2c_cfg_t* cfg = (swi2c_cfg_t*)port;
    gpio_put(cfg->scl, level == LOW ? 0 : 1);
}

static pin_lvl_e swi2c_get_sda(void* port)
{
    swi2c_cfg_t* cfg = (swi2c_cfg_t*)port;
    return gpio_get(cfg->sda) == 0 ? LOW : HIGH;
}

static pin_lvl_e swi2c_get_scl(void* port)
{
    swi2c_cfg_t* cfg = (swi2c_cfg_t*)port;
    return gpio_get(cfg->scl) == 0 ? LOW : HIGH;
}

void swi2c_cfg_sda(void* port, pin_dir_e dir)
{
    swi2c_cfg_t* cfg = (swi2c_cfg_t*)port;
    gpio_set_dir(cfg->sda, dir == OUT ? GPIO_OUT : GPIO_IN);
}

void swi2c_cfg_scl(void* port, pin_dir_e dir)
{
    swi2c_cfg_t* cfg = (swi2c_cfg_t*)port;
    gpio_set_dir(cfg->scl, dir == OUT ? GPIO_OUT : GPIO_IN);
}

i2c_bus_t* swi2c_init(void)
{
    static swi2c_cfg_t cfg = {
        .name = "swi2c-bus",
        .sda  = 4,
        .scl  = 5,
    };

    static swi2c_drv_t drv = {
        .port     = &cfg,
        .cfg_scl  = swi2c_cfg_scl,
        .cfg_sda  = swi2c_cfg_sda,
        .set_sda  = swi2c_set_sda,
        .set_scl  = swi2c_set_scl,
        .get_sda  = swi2c_get_sda,
        .get_scl  = nullptr,  // swi2c_get_scl,
        .delay    = sleep_us,
        .interval = 2,
        .timeout  = 5,
    };

    static i2c_bus_t bus = {
        .drv = &drv,
        .ops = &g_swi2c_ops,
    };

    println("%s", cfg.name);

    gpio_init(cfg.sda);
    gpio_init(cfg.scl);

    gpio_pull_up(cfg.sda);
    gpio_pull_up(cfg.scl);

    return &bus;
}

static err_t hwi2c_master_xfer(i2c_bus_t* bus, __IN i2c_msg_t* msgs, __IN uint16_t cnt)
{
    i2c_msg_t* msg;

    hwi2c_drv_t* drv = (hwi2c_drv_t*)(bus->drv);
    hwi2c_cfg_t* cfg = (hwi2c_cfg_t*)(drv->port);

    struct i2c_inst* i2c = (struct i2c_inst*)(cfg->handle);

    uint16_t idx = 0;

    int err;

    for (idx = 0; idx < cnt; ++idx) {
        msg = &msgs[idx];

        if (msg->flgs & I2CMST_NO_START) {  // necessary
            i2c->restart_on_next = false;
        }

        if (msg->flgs & I2CMST_RAED) {
            err = i2c_read_timeout_per_char_us(i2c, msg->addr, msg->dat, msg->len, msg->flgs & I2CMST_NO_STOP, drv->timeout);
            if (err == PICO_ERROR_GENERIC) {
                return ERR_TIMEOUT;
            }
        } else {
            err = i2c_write_timeout_us(i2c, msg->addr, msg->dat, msg->len, msg->flgs & I2CMST_NO_STOP, drv->timeout);
            if (err == PICO_ERROR_GENERIC) {
                return ERR_TIMEOUT;
            }
        }
    }

    return SUCCESS;
}

i2c_bus_t* hwi2c_init()
{
    static hwi2c_cfg_t cfg = {
        .name    = "hwi2c-bus",
        .handle  = i2c0,
        .sda     = 4,
        .scl     = 5,
        .bitrate = 400 * 1000U,
    };

    static i2c_ops_t hwi2c_ops = {
        .xfer  = hwi2c_master_xfer,
        .ioctl = nullptr,  // set bitrate or ...
    };

    static hwi2c_drv_t drv = {
        .port    = &cfg,
        .timeout = 100,  // us
    };

    static i2c_bus_t bus = {
        .drv = &drv,
        .ops = &hwi2c_ops,
    };

    i2c_init(cfg.handle, cfg.bitrate);

    gpio_init(cfg.sda);
    gpio_init(cfg.scl);

    gpio_set_function(cfg.sda, GPIO_FUNC_I2C);
    gpio_set_function(cfg.scl, GPIO_FUNC_I2C);

    gpio_pull_up(cfg.sda);
    gpio_pull_up(cfg.scl);

    return &bus;
}

int main()
{
    stdio_init_all();

#if 0
    i2c_bus_t* bus = swi2c_init();
#else
    i2c_bus_t* bus = hwi2c_init();
#endif

    switch (4) {  // demo switch
        default:
        case 1: {
            void scanner_demo(i2c_bus_t * bus);
            scanner_demo(bus);
            break;
        }
        case 2: {
            void xfer_demo(i2c_bus_t * bus);
            xfer_demo(bus);
            break;
        }
        case 3: {
            void ssd1306_demo(i2c_bus_t * bus);
            ssd1306_demo(bus);
            break;
        }
        case 4: {
            void mpu6050_demo(i2c_bus_t * bus);
            getchar();  // wait a char and then run demo
            mpu6050_demo(bus);
            break;
        }
    }

    return 0;
}

void scanner_demo(i2c_bus_t* bus)
{
    while (1) {
        i2cmst_scanner(bus);
        sleep_ms(2000);
    }
}

void xfer_demo(i2c_bus_t* bus)
{
    // data transmit test (use logic-analyzer to view it)

    while (1) {
        printf(".");

        i2c_msg_t msg = {
            .addr = 0x10,
            .dat  = (uint8_t[]){0x11, 0x22, 0x33, 0x44},
            .len  = 4,
            .flgs = I2CMST_WRITE | I2CMST_ADDR_7BIT,
        };

        i2cmst_xfer(bus, &msg, 1);
        sleep_ms(200);

        msg.flgs |= I2CMST_IGNORE_NACK;
        i2cmst_xfer(bus, &msg, 1);
        sleep_ms(2000);
    }
}

void ssd1306_init(i2c_dev_t* dev)
{
    uint8_t cmd[] = {
        0xae,
        0xd5,
        0x80,
        0xa8,
        0x3f,
        0xd3,
        0x00,
        0x40,
        0x20,
        0x00,
        0xa1,
        0xc8,
        0xda,
        0x12,
        0x81,
        0xcf,
        0xd9,
        0xf1,
        0xdb,
        0x30,
        0x8d,
        0x14,
        0x2e,
        0xa4,
        0xa6,
        0xaf,
    };

    i2cdev_write_bytes(dev, 0x00, cmd, sizeof(cmd));
}

void ssd1306_fill(i2c_dev_t* dev, uint8_t color)
{
    uint8_t x0 = 0;
    uint8_t x1 = 127;
    uint8_t y0 = 0;
    uint8_t y1 = 7;

    uint8_t cmd[] = {
        0x21, x0, x1,  // col
        0x22, y0, y1,  // row
    };

    i2cdev_write_bytes(dev, 0x00, cmd, sizeof(cmd));

    uint8_t dat[16];
    uint8_t cnt = sizeof(dat);
    while (cnt--) {
        dat[cnt] = color;
    }

    cnt = 64;
    while (cnt--) {
        i2cdev_write_bytes(dev, 0x40, dat, sizeof(dat));
    }
}

void ssd1306_demo(i2c_bus_t* bus)
{
    // ssd1306
    i2c_dev_t dev = {
        .bus  = bus,
        .addr = 0x3C,
    };

    ssd1306_init(&dev);

    uint8_t color = 0x0F;

    while (1) {
        ssd1306_fill(&dev, color);
        color = ~color;
        sleep_ms(2000);
    }
}

void mpu6050_demo(i2c_bus_t* bus)
{
    // mpu6050
    i2c_dev_t dev = {
        .bus  = bus,
        .addr = 0x68,
    };

    i2cdev_write_byte(&dev, 0x6B, 0x00);
    printf("ax\tay\taz\tgx\tgy\tgz\ttmp\n");

    while (1) {
#if 1
        uint8_t buf[14];
        i2cdev_read_bytes(&dev, 0x3B, buf, 14);
        float ax  = (int16_t)((buf[0] << 8) | buf[1]) / 16384.00;
        float ay  = (int16_t)((buf[2] << 8) | buf[3]) / 16384.00;
        float az  = (int16_t)((buf[4] << 8) | buf[5]) / 16384.00;
        float tmp = (int16_t)((buf[6] << 8) | buf[7]) / 340.00 + 36.53;
        float gx  = (int16_t)((buf[8] << 8) | buf[9]) / 65.5;  // 16.4
        float gy  = (int16_t)((buf[10] << 8) | buf[11]) / 65.5;
        float gz  = (int16_t)((buf[12] << 8) | buf[13]) / 65.5;

        printf("%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n",
               ax, ay, az, gx, gy, gz, tmp);
#endif
        sleep_ms(100);
    }
}
