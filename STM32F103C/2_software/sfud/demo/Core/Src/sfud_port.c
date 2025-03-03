#include "spimst.h"
#include "pinmap.h"
#include "logger.h"
#include "spi.h"
#include "sfud.h"
#include "sfud_cfg.h"
#include <stdarg.h>
#include <stdio.h>

#define LOG_LOCAL_TAG   "SFUD"
#define LOG_LOCAL_LEVEL LOG_LEVEL_VERBOSE

#ifndef W25QXX_SPI_TIMING
#define W25QXX_SPI_TIMING (SPI_FLAG_4WIRE | SPI_FLAG_CPHA_1EDGE | SPI_FLAG_CPOL_LOW | SPI_FLAG_MSBFIRST | SPI_FLAG_DATAWIDTH_8B | SPI_FLAG_CS_ACTIVE_LOW | SPI_FLAG_FAST_CLOCK_ENABLE)
#endif

spi_mst_t spi = {
	 // .SPIx = &hspi1,
		.MISO = {SPI_MISO_PIN},
		.MOSI = {SPI_MOSI_PIN},
		.SCLK = {SPI_SCLK_PIN},
		.CS   = {FLASH_CS_PIN},
};

static char log_buf[256];

void sfud_log_debug(const char *file, const long line, const char *format, ...);

static void spi_lock(const sfud_spi *spi) {
   // __disable_irq();
}

static void spi_unlock(const sfud_spi *spi) {
   // __enable_irq();
}

/**
 * SPI write data then read data
 */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf, size_t read_size) {
    sfud_err result = SFUD_SUCCESS;
    uint8_t send_data, read_data;
    spi_mst_t* spi_dev = (spi_mst_t*) spi->user_data;

    if (write_size) {
        SFUD_ASSERT(write_buf);
    }
    if (read_size) {
        SFUD_ASSERT(read_buf);
    }
		
		SPI_Master_Select(spi_dev);
		
		if (SPI_Master_TransmitBlock(spi_dev, write_buf, write_size) != ERR_NONE) return SFUD_ERR_WRITE;
		if (SPI_Master_ReceiveBlock(spi_dev, read_buf, read_size) != ERR_NONE) return SFUD_ERR_READ;

		SPI_Master_Deselect(spi_dev);

    return SFUD_SUCCESS;
}

/* about 100 microsecond delay */
static void retry_delay_100us(void) {
    uint32_t delay = 120;
    while(delay--);
}

sfud_err sfud_spi_port_init(sfud_flash *flash) {
    sfud_err result = SFUD_SUCCESS;

    switch (flash->index) {
    case SFUD_W25QXX_DEVICE_INDEX: {
			  SPI_Master_Init(&spi, 100000, SPI_DUTYCYCLE_50_50, W25QXX_SPI_TIMING | SPI_FLAG_SOFT_CS);
        /* 同步 Flash 移植所需的接口及数据 */
        flash->spi.wr = spi_write_read;
        flash->spi.lock = spi_lock;
        flash->spi.unlock = spi_unlock;
        flash->spi.user_data = &spi;
        /* about 100 microsecond delay */
        flash->retry.delay = retry_delay_100us;
        /* adout 60 seconds timeout */
        flash->retry.times = 60 * 10000;

        break;
    }
    }

    return result;
}

/**
 * This function is print debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 */
void sfud_log_debug(const char *file, const long line, const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    LOGD("(%s:%ld) %s", file, line, log_buf);
    va_end(args);
}

/**
 * This function is print routine info.
 *
 * @param format output format
 * @param ... args
 */
void sfud_log_info(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    LOGI("%s", log_buf);
    va_end(args);
}
