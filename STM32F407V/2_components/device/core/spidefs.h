#ifndef __SPI_DEFS_H__
#define __SPI_DEFS_H__

#include "typebasic.h"
#include "pinctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

/**
 * @brief 数据线模式
 */
#define SPI_FLAG_WIRE_Pos 0U
#define SPI_FLAG_WIRE_Len 1U
#define SPI_FLAG_WIRE_Msk BITMASK16(SPI_FLAG_WIRE_Pos, SPI_FLAG_WIRE_Len)
#define SPI_FLAG_3WIRE    (0U << SPI_FLAG_WIRE_Pos)
#define SPI_FLAG_4WIRE    (1U << SPI_FLAG_WIRE_Pos)

/**
 * @brief 时钟极性(CPOL,clock polarity)：时钟线空闲电平
 * @note  clock steady state
 */
#define SPI_FLAG_CPOL_Pos  (SPI_FLAG_WIRE_Pos + SPI_FLAG_WIRE_Len)
#define SPI_FLAG_CPOL_Len  1U
#define SPI_FLAG_CPOL_Msk  BITMASK16(SPI_FLAG_CPOL_Pos, SPI_FLAG_CPOL_Len)
#define SPI_FLAG_CPOL_LOW  (0U << SPI_FLAG_CPOL_Pos)
#define SPI_FLAG_CPOL_HIGH (1U << SPI_FLAG_CPOL_Pos)

/**
 * @brief 时钟相位(CPHA,clock phase)：奇/偶数边缘采样
 * @note  clock active edge for the bit capture
 */
#define SPI_FLAG_CPHA_Pos   (SPI_FLAG_CPOL_Pos + SPI_FLAG_CPOL_Len)
#define SPI_FLAG_CPHA_Len   1U
#define SPI_FLAG_CPHA_Msk   BITMASK16(SPI_FLAG_CPHA_Pos, SPI_FLAG_CPHA_Len)
#define SPI_FLAG_CPHA_1EDGE (0U << SPI_FLAG_CPHA_Pos)
#define SPI_FLAG_CPHA_2EDGE (1U << SPI_FLAG_CPHA_Pos)

/**
 * @brief 位时序
 */
#define SPI_FLAG_FIRSTBIT_Pos (SPI_FLAG_CPHA_Pos + SPI_FLAG_CPHA_Len)
#define SPI_FLAG_FIRSTBIT_Len 2U
#define SPI_FLAG_FIRSTBIT_Msk BITMASK16(SPI_FLAG_FIRSTBIT_Pos, SPI_FLAG_FIRSTBIT_Len)
#define SPI_FLAG_LSBFIRST     (0U << SPI_FLAG_FIRSTBIT_Pos) /*!< 低位在前 */
#define SPI_FLAG_MSBFIRST     (1U << SPI_FLAG_FIRSTBIT_Pos) /*!< 高位在前 */

/**
 * @brief 数据位宽
 */
#define SPI_FLAG_DATAWIDTH_Pos (SPI_FLAG_FIRSTBIT_Pos + SPI_FLAG_FIRSTBIT_Len)
#define SPI_FLAG_DATAWIDTH_Len 2U
#define SPI_FLAG_DATAWIDTH_Msk BITMASK16(SPI_FLAG_DATAWIDTH_Pos, SPI_FLAG_DATAWIDTH_Len)
#define SPI_FLAG_DATAWIDTH_8B  (0U << SPI_FLAG_DATAWIDTH_Pos)
#define SPI_FLAG_DATAWIDTH_16B (1U << SPI_FLAG_DATAWIDTH_Pos)
#define SPI_FLAG_DATAWIDTH_32B (2U << SPI_FLAG_DATAWIDTH_Pos)

/**
 * @brief 片选模式
 */
#define SPI_FLAG_CS_MODE_Pos (SPI_FLAG_DATAWIDTH_Pos + SPI_FLAG_DATAWIDTH_Len)
#define SPI_FLAG_CS_MODE_Len 2U
#define SPI_FLAG_CS_MODE_Msk BITMASK16(SPI_FLAG_CS_MODE_Pos, SPI_FLAG_CS_MODE_Len)
#define SPI_FLAG_NONE_CS     (0U << SPI_FLAG_CS_MODE_Pos)
#define SPI_FLAG_SOFT_CS     (1U << SPI_FLAG_CS_MODE_Pos)
#define SPI_FLAG_HADR_CS     (2U << SPI_FLAG_CS_MODE_Pos)

/**
 * @brief 片选激活电平
 */
#define SPI_FLAG_CS_ACTIVE_LEVEL_Pos (SPI_FLAG_CS_MODE_Pos + SPI_FLAG_CS_MODE_Len)
#define SPI_FLAG_CS_ACTIVE_LEVEL_Len 1U
#define SPI_FLAG_CS_ACTIVE_LEVEL_Msk BITMASK16(SPI_FLAG_CS_ACTIVE_LEVEL_Pos, SPI_FLAG_CS_ACTIVE_LEVEL_Len)
#define SPI_FLAG_CS_ACTIVE_LOW       (0U << SPI_FLAG_CS_ACTIVE_LEVEL_Pos)
#define SPI_FLAG_CS_ACTIVE_HIGH      (1U << SPI_FLAG_CS_ACTIVE_LEVEL_Pos)

/**
 * @brief 快速时钟模式 [swspi]
 */
#define SPI_FLAG_FAST_CLOCK_Pos     (SPI_FLAG_CS_ACTIVE_LEVEL_Pos + SPI_FLAG_CS_ACTIVE_LEVEL_Len)
#define SPI_FLAG_FAST_CLOCK_Len     1U
#define SPI_FLAG_FAST_CLOCK_Msk     BITMASK16(SPI_FLAG_FAST_CLOCK_Pos, SPI_FLAG_FAST_CLOCK_Len)
#define SPI_FLAG_FAST_CLOCK_DISABLE (0U << SPI_FLAG_FAST_CLOCK_Pos)
#define SPI_FLAG_FAST_CLOCK_ENABLE  (1U << SPI_FLAG_FAST_CLOCK_Pos)

/**
 * @brief
 *
 */

typedef enum {
    SPI_DUTYCYCLE_33_67 = 84,   // 255*0.33
    SPI_DUTYCYCLE_50_50 = 127,  // 255*0.5
} spi_duty_cycle_e;

/**
 * @brief
 *
 */

typedef struct spimst_ops spimst_ops_t;

typedef struct spimst {
    __IN const pin_t MOSI;  // master input, slave output
    __IN const pin_t MISO;  // master output, slave input
    __IN const pin_t SCLK;  // serial clock line
    __IN const pin_t CS;    // chip select @SPI_FLAG_SOFT_CS or @SPI_FLAG_HADR_CS
    __IN const void* SPIx;

    u16 u16TimingConfig;

#if CONFIG_SWSPI_MODULE_SW

    u32 u32FirstCycleUs;   // [swspi only] first edge duration @SPI_FLAG_FAST_CLOCK_DISABLE
    u32 u32SecondCycleUs;  // [swspi only] second edge duration @SPI_FLAG_FAST_CLOCK_DISABLE

#endif

    const spimst_ops_t* pOps;
} spi_mst_t;

struct spimst_ops {
    status_t (*Init)(spi_mst_t* pHandle, u32 u32ClockSpeedHz, spi_duty_cycle_e eDutyCycle, u16 u16Flags);
    status_t (*TransmitBlock)(spi_mst_t* pHandle, const u8* cpu8TxData, u16 u16Size);
    status_t (*ReceiveBlock)(spi_mst_t* pHandle, u8* pu8RxData, u16 u16Size);
    status_t (*TransmitReceiveBlock)(spi_mst_t* pHandle, const u8* cpu8TxData, u8* pu8RxData, u16 u16Size);
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
