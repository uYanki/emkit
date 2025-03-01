#ifndef __SPI_MT6701_H__
#define __SPI_MT6701_H__

#include "spimst.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define MT6701_SPI_TIMING (SPI_FLAG_3WIRE | SPI_FLAG_CPOL_HIGH | SPI_FLAG_CPHA_2EDGE | SPI_FLAG_MSBFIRST | SPI_FLAG_DATAWIDTH_8B | SPI_FLAG_CS_ACTIVE_LOW)

typedef struct {
    spi_mst_t* hSPI;
} spi_mt6701_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void     MT6701_Init(spi_mt6701_t* pHandle);
uint16_t MT6701_ReadAngle(spi_mt6701_t* pHandle);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void MT6701_Test(void);
#endif

#endif
