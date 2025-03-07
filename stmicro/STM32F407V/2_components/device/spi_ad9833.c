#include "spi_ad9833.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "as9833"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

#define POW_2_28        0x10000000UL  // 2^28
#define CHANGE_PER_DEG  11.37         // 4096/360

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static inline void AD9833_Write(spi_ad9833_t* pHandle, uint16_t u16Data)
{
    SPI_Master_Select(pHandle->hSPI);

    switch (AD9833_SPI_TIMING & SPI_FLAG_DATAWIDTH_Msk)
    {
        case SPI_FLAG_DATAWIDTH_8B:
        {
            uint8_t u8Data[2];

            u8Data[0] = u16Data >> 8;
            u8Data[1] = u16Data & 0xFF;

            SPI_Master_TransmitBlock(pHandle->hSPI, &u8Data[0], 2);

            break;
        }

        case SPI_FLAG_DATAWIDTH_16B:
        {
            SPI_Master_TransmitBlock(pHandle->hSPI, (uint8_t*)&u16Data, 1);

            break;
        }

        default:
        {
            ASSERT(0, "unsupported");
            break;
        }
    }

    SPI_Master_Deselect(pHandle->hSPI);
}

void AD9833_Init(spi_ad9833_t* pHandle)
{
    SPI_Master_Deselect(pHandle->hSPI);

    AD9833_Reset(pHandle);
    DelayBlockMs(1);

    AD9833_SetFrequency(pHandle, AD9833_FREQ0_SELECT, 0);
    AD9833_SetFrequency(pHandle, AD9833_FREQ1_SELECT, 0);
    AD9833_SetPhase(pHandle, AD9833_PHASE0_SELECT, 0);
    AD9833_SetPhase(pHandle, AD9833_PHASE1_SELECT, 0);
    AD9833_Write(pHandle, 0x00);  // clear reset bit
    AD9833_Write(pHandle, AD9833_CMD_SLEEP);
}

void AD9833_Reset(spi_ad9833_t* pHandle)
{
    AD9833_Write(pHandle, AD9833_CMD_RESET);
}

void AD9833_SetWaveform(spi_ad9833_t* pHandle, ad9833_waveform_e wave)
{
    AD9833_Write(pHandle, wave);
}

void AD9833_SetFrequency(spi_ad9833_t* pHandle, ad9833_freq_select_e eFreqDst, float32_t f32Frequency /*hz*/)
{
    uint32_t v = (float32_t)f32Frequency * POW_2_28 / CONFIG_AD9833_FMCLK;

    uint16_t u16FreqLo = eFreqDst | (v & 0x3FFF);          // lower 14 bits
    uint16_t u16FreqHi = eFreqDst | ((v >> 14) & 0x3FFF);  // upper 14 bits

    AD9833_Write(pHandle, u16FreqLo);
    AD9833_Write(pHandle, u16FreqHi);
}

void AD9833_SetPhase(spi_ad9833_t* pHandle, ad9833_phase_select_e eFreqDst, float32_t f32PhaseShift)
{
    f32PhaseShift = CLAMP(0, f32PhaseShift, 360);
    uint16_t v    = f32PhaseShift * CHANGE_PER_DEG;  // [0,4095]
    AD9833_Write(pHandle, eFreqDst | v);
}

float32_t AD9833_GetResolution(spi_ad9833_t* pHandle)
{
    return (float32_t)CONFIG_AD9833_FMCLK / POW_2_28;
}

void AD9833_SetOutput(spi_ad9833_t* pHandle, ad9833_freq_output_e freq, ad9833_phase_output_e phase)
{
    AD9833_Write(pHandle, freq);
    AD9833_Write(pHandle, phase);
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

void AD9833_Test(void)
{
    spi_mst_t spi = {
        .MISO = {GPIOA, GPIO_PIN_5}, /* SDATA */
        .MOSI = {GPIOA, GPIO_PIN_5},
        .SCLK = {GPIOA, GPIO_PIN_6}, /* SCLK */
        .CS   = {GPIOA, GPIO_PIN_3}, /* FSYNC */
    };

    spi_ad9833_t ad9833 = {
        .hSPI = &spi,
    };

    SPI_Master_Init(&spi, 1000000, SPI_DUTYCYCLE_50_50, AD9833_SPI_TIMING | SPI_FLAG_SOFT_CS);

    AD9833_Init(&ad9833);

    AD9833_SetFrequency(&ad9833, AD9833_FREQ0_SELECT, 40000);
    AD9833_SetPhase(&ad9833, AD9833_PHASE0_SELECT, 0);
    AD9833_SetOutput(&ad9833, AD9833_FREQ0_OUTPUT, AD9833_PHASE0_OUTPUT);

    while (1)
    {
        AD9833_SetWaveform(&ad9833, AD9833_WAVEFORM_SINE);
        DelayBlockMs(2000);
        AD9833_SetWaveform(&ad9833, AD9833_WAVEFORM_HALF_SQUARE);
        DelayBlockMs(2000);
        AD9833_SetWaveform(&ad9833, AD9833_WAVEFORM_TRIANGLE);
        DelayBlockMs(2000);
        AD9833_SetWaveform(&ad9833, AD9833_WAVEFORM_SQUARE);
        DelayBlockMs(2000);
    }
}

#endif
