#ifndef __SPI_NRF24L01_H__
#define __SPI_NRF24L01_H__

/**
 * @brief NRF24L01 2.4GHz Wireless Transceiver
 * @ref https://github.com/nRF24/RF24
 */

#include "spimst.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define NRF24L01_SPI_TIMING (SPI_FLAG_4WIRE | SPI_FLAG_CPHA_1EDGE | SPI_FLAG_CPOL_LOW | SPI_FLAG_MSBFIRST | SPI_FLAG_DATAWIDTH_8B | SPI_FLAG_CS_ACTIVE_LOW | SPI_FLAG_FAST_CLOCK_ENABLE)

/**
 * @brief Power Amplifier level
 * Power Amplifier level. The units dBm (decibel-milliwatts or dB<sub>mW</sub>)
 * represents a logarithmic signal loss.
 */
typedef enum {
    /**
     * (0) represents:
     * nRF24L01 | Si24R1 with<br>lnaEnabled = 1 | Si24R1 with<br>lnaEnabled = 0
     * :-------:|:-----------------------------:|:----------------------------:
     *  -18 dBm | -6 dBm | -12 dBm
     */
    RF24_PA_MIN = 0,
    /**
     * (1) represents:
     * nRF24L01 | Si24R1 with<br>lnaEnabled = 1 | Si24R1 with<br>lnaEnabled = 0
     * :-------:|:-----------------------------:|:----------------------------:
     *  -12 dBm | 0 dBm | -4 dBm
     */
    RF24_PA_LOW,
    /**
     * (2) represents:
     * nRF24L01 | Si24R1 with<br>lnaEnabled = 1 | Si24R1 with<br>lnaEnabled = 0
     * :-------:|:-----------------------------:|:----------------------------:
     *  -6 dBm | 3 dBm | 1 dBm
     */
    RF24_PA_HIGH,
    /**
     * (3) represents:
     * nRF24L01 | Si24R1 with<br>lnaEnabled = 1 | Si24R1 with<br>lnaEnabled = 0
     * :-------:|:-----------------------------:|:----------------------------:
     *  0 dBm | 7 dBm | 4 dBm
     */
    RF24_PA_MAX,
    /**
     * (4) This should not be used and remains for backward compatibility.
     */
    RF24_PA_ERROR
} rf24_pa_dbm_e;

/**
 * Data rate.  How fast data moves through the air. Units are in bits per second (bps).
 */
typedef enum {
    RF24_1MBPS = 0,
    RF24_2MBPS,
    RF24_250KBPS,
} rf24_datarate_e;

/**
 * @brief CRC Length.  How big (if any) of a CRC is included.
 * The length of a CRC checksum that is used (if any). Cyclical Redundancy
 * Checking (CRC) is commonly used to ensure data integrity.
 */
typedef enum {
    RF24_CRC_DISABLED = 0,
    RF24_CRC_8,
    RF24_CRC_16,
} rf24_crc_len_e;

typedef struct {
    __IN spi_mst_t*  hSPI;
    __IN const pin_t CE;  // The pin attached to Chip Enable on the RF module

    bool     bPlusVer;                /* False for RF24L01 and true for RF24L01P */
    uint8_t  u8PayloadSize;           /**< Fixed size of payloads */
    bool     bAckPayload;             /**< Whether there is an ack payload waiting */
    uint8_t  u8AddrWidth;             /**< The address width to use (3, 4 or 5 bytes). */
    bool     bDynamicPayloadsEnabled; /**< Whether dynamic payloads are enabled. */
    uint8_t  u8AckPayloadLength;      /**< Dynamic size of pending ack payload. */
    uint64_t u64Pipe0ReadingAddress;  /**< Last address set on pipe 0 for reading. */

} spi_nrf24l01_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void NRF24L01_Init(spi_nrf24l01_t* pHandle);
/**
 * Checks if the chip is connected to the SPI bus
 */
bool NRF24L01_IsChipConnected(spi_nrf24l01_t* pHandle);

/**
 * @brief Start listening on the pipes opened for reading.
 */
void NRF24L01_StartListening(spi_nrf24l01_t* pHandle);

/**
 * @brief Stop listening for incoming messages
 */
void NRF24L01_StopListening(spi_nrf24l01_t* pHandle);

/**
 * @brief Write to the open writing pipe

 * @param[in] cpu8Data Pointer to the data to be sent
 * @param[in] u8Len Number of bytes to be sent. The maximum size of data written
 * is the fixed payload size, see NRF24L01_GetPayloadSize().  However, you can
 * write less, and the remainderwill just be filled with zeroes.
 *
 * @return True if the payload was delivered successfully false if not
 */
bool NRF24L01_WritePacket(spi_nrf24l01_t* pHandle, const uint8_t* cpu8Data, uint8_t u8Len);

/**
 * @brief Read the payload
 *
 * @param[in] pu8Data Pointer to a buffer where the data should be written
 * @param[in] u8Len Maximum number of bytes to read into the buffer. The size of
 * data read is the fixed payload size, see NRF24L01_GetPayloadSize()
 *
 * @return True if the payload was delivered successfully false if not
 */
bool NRF24L01_ReadPacket(spi_nrf24l01_t* pHandle, uint8_t* pu8Data, uint8_t u8Len);

/**
 * @brief Open a pipe for writing. Old addressing format retained for compatibility.
 *
 * @param[in] u64Address The 24, 32 or 40 bit address of the pipe to open.
 */
void NRF24L01_OpenWritingPipe(spi_nrf24l01_t* pHandle, uint64_t u64Address);

/**
 * @brief Open a pipe for reading
 *
 * @param[in] u8Pipe Which pipe to open. Only pipe numbers 0-5 are available,
 * an address assigned to any pipe number not in that range will be ignored.
 * @param[in] u64Address The 24, 32 or 40 bit address of the pipe to open.
 *
 */
void NRF24L01_OpenReadingPipe(spi_nrf24l01_t* pHandle, uint8_t u8Pipe, uint64_t u64Address);

/**
 * @brief Set the number and delay of retries upon failed submit
 *
 * @param u8Delay How long to wait between each retry, in multiples of 250us,
 * max is 15.  0 means 250us, 15 means 4000us.
 * @param u8Count How many retries before giving up, max 15
 */
void NRF24L01_SetRetries(spi_nrf24l01_t* pHandle, uint8_t u8Delay, uint8_t u8Count);

/**
 * @brief Set RF communication channel
 *
 * @param u8Channel Which RF channel to communicate on, 0-127
 */
void NRF24L01_SetChannel(spi_nrf24l01_t* pHandle, uint8_t u8Channel);

/**
 * @brief Set Static Payload Size
 *
 * @param[in] size The number of bytes in the payload
 */
void NRF24L01_SetPayloadSize(spi_nrf24l01_t* pHandle, uint8_t u8Size);

/**
 * @brief Get Static Payload Size
 *
 * @return The number of bytes in the payload
 */
uint8_t NRF24L01_GetPayloadSize(spi_nrf24l01_t* pHandle);

/**
 * @brief Get Dynamic Payload Size
 *
 * @return Payload length of last-received dynamic payload
 */
uint8_t NRF24L01_GetDynamicPayloadSize(spi_nrf24l01_t* pHandle);

/**
 * @brief Enable custom payloads on the acknowledge packets
 */
void NRF24L01_EnableAckPayload(spi_nrf24l01_t* pHandle);

/**
 * @brief  Disable custom payloads on the acknowledge packets
 */
void NRF24L01_DisableAckPayload(spi_nrf24l01_t* pHandle);

/**
 * @brief Enable dynamically-sized payloads
 *
 * @note This way you don't always have to send large packets just to send them
 * once in a while.  This enables dynamic payloads on ALL pipes.
 */
void NRF24L01_EnableDynamicPayloads(spi_nrf24l01_t* pHandle);

/**
 * @brief Determine whether the hardware is an nRF24L01+ or not.
 *
 * @return true if the hardware is nRF24L01+ (or compatible) and false
 * if its not.
 */
bool NRF24L01_IsPlusVariant(spi_nrf24l01_t* pHandle);

/**
 * @brief Enable or disable auto-acknowlede packets
 *
 * This is enabled by default, so it's only needed if you want to turn
 * it off for some reason.
 *
 * @param[in] bEnable Whether to enable (true) or disable (false) auto-acks
 */
void NRF24L01_SetAutoAck(spi_nrf24l01_t* pHandle, bool bEnable);

/**
 * @brief Enable or disable auto-acknowlede packets on a per pipeline basis.
 *
 * @param[in] u8Pipe Which pipeline to modify
 * @param[in] bEnable Whether to enable (true) or disable (false) auto-acks
 */
void NRF24L01_SetPipeAutoAck(spi_nrf24l01_t* pHandle, uint8_t u8Pipe, bool bEnable);

/**
 * @brief Set Power Amplifier (PA) level to one of four levels.
 *
 * Relative mnemonics have been used to allow for future PA level
 * changes. According to 6.5 of the nRF24L01+ specification sheet,
 * they translate to: RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm,
 * RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
 *
 * @param[in] eLevel Desired PA level.
 */
void NRF24L01_SetPALevel(spi_nrf24l01_t* pHandle, rf24_pa_dbm_e eLevel);

/**
 * @brief Fetches the current PA level.
 *
 * @return Returns a value from the rf24_pa_dbm_e enum describing
 * the current PA setting. Please remember, all values represented
 * by the enum mnemonics are negative dBm. See setPALevel for
 * return value descriptions.
 */
rf24_pa_dbm_e NRF24L01_GetPALevel(spi_nrf24l01_t* pHandle);

/**
 * @brief Set the transmission data rate
 *
 * @warning setting RF24_250KBPS will fail for non-plus units
 *
 * @param eSpeed RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
 * @return true if the change was successful
 */
bool NRF24L01_SetDataRate(spi_nrf24l01_t* pHandle, rf24_datarate_e eSpeed);

/**
 * @brief Fetches the transmission data rate
 *
 * @return Returns the hardware's currently configured datarate. The value
 * is one of 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS, as defined in the
 * rf24_datarate_e enum.
 */
rf24_datarate_e NRF24L01_GetDataRate(spi_nrf24l01_t* pHandle);

/**
 * @brief Set the CRC length
 *
 * @param eLength RF24_CRC_8 for 8-bit or RF24_CRC_16 for 16-bit
 */
void NRF24L01_SetCRCLength(spi_nrf24l01_t* pHandle, rf24_crc_len_e eLength);

/**
 * @brief Get the CRC length
 *
 * @return RF24_DISABLED if disabled or RF24_CRC_8 for 8-bit or RF24_CRC_16 for 16-bit
 */
rf24_crc_len_e NRF24L01_GetCRCLength(spi_nrf24l01_t* pHandle);

/**
 * @brief Disable CRC validation
 */
void NRF24L01_DisableCRC(spi_nrf24l01_t* pHandle);

/**
 * @brief Enter low-power mode
 *
 * To return to normal power mode, either NRF24L01_WritePacket() some data or
 * startListening, or NRF24L01_PowerUp().
 */
void NRF24L01_PowerDown(spi_nrf24l01_t* pHandle);

/**
 * @brief Leave low-power mode - making radio more responsive
 *
 * To return to low power mode, call NRF24L01_PowerDown().
 */
void NRF24L01_PowerUp(spi_nrf24l01_t* pHandle);

/**
 * @brief Test whether there are bytes available to be read
 * @return True if there is a payload available, false if none is
 */
bool NRF24L01_IsAvailable(spi_nrf24l01_t* pHandle);

/**
 * @brief Test whether there are bytes available to be read
 *
 * @param[out] pu8Pipe Which pipe has the payload available
 * @return True if there is a payload available, false if none is
 */
bool NRF24L01_IsPipeAvailable(spi_nrf24l01_t* pHandle, uint8_t* pu8Pipe);

/**
 * @brief Non-blocking write to the open writing pipe
 *
 * Just like NRF24L01_WritePacket(), but it returns immediately. To find out what happened
 * to the send, catch the IRQ and then call NRF24L01_WhatHappened().
 *
 * @param[in] cpu8Data Pointer to the data to be sent
 * @param[in] u8Len Number of bytes to be sent
 * @return True if the payload was delivered successfully false if not
 */
bool NRF24L01_WritePacketNonBlock(spi_nrf24l01_t* pHandle, const uint8_t* cpu8Data, uint8_t u8Len, bool bBroadcast);

/**
 * @brief Write an ack payload for the specified pipe
 *
 * The next time a message is received on @p pipe, the data in @p buf will
 * be sent back in the acknowledgement.
 *
 * @param u8Pipe Which pipe# (typically 1-5) will get this response.
 * @param cpu8Data Pointer to data that is sent
 * @param u8Len Length of the data to send, up to 32 bytes max.  Not affected
 * by the static payload set by NRF24L01_SetPayloadSize().
 */
void NRF24L01_WriteAckPayload(spi_nrf24l01_t* pHandle, uint8_t u8Pipe, const uint8_t* cpu8Data, uint8_t u8Len);

/**
 * @brief Call this when you get an interrupt to find out why
 *
 * Tells you what caused the interrupt, and clears the state of
 * interrupts.
 *
 * @param[out] pbTxOk The send was successful (TX_DS)
 * @param[out] pbTxFali The send failed, too many retries (MAX_RT)
 * @param[out] pbRxRdy There is a message waiting to be read (RX_DS)
 */
void NRF24L01_WhatHappened(spi_nrf24l01_t* pHandle, bool* pbTxOk, bool* pbTxFali, bool* pbRxRdy);

/**
 * @brief Test whether there was a carrier on the line for the
 * previous listening period.
 *
 * Useful to check for interference on the current channel.
 *
 * @return true if was carrier, false if not
 */
bool NRF24L01_TestCarrier(spi_nrf24l01_t* pHandle);

/**
 * @brief Test whether a signal (carrier or otherwise) greater than
 * or equal to -64dBm is present on the channel. Valid only
 * on nRF24L01P (+) hardware. On nRF24L01, use NRF24L01_TestCarrier().
 *
 * Useful to check for interference on the current channel and
 * channel hopping strategies.
 *
 * @return true if signal => -64dBm, false if not
 */
bool NRF24L01_TestRPD(spi_nrf24l01_t* pHandle);

/**
 * @brief Print a giant block of debugging information to stdout
 */
void NRF24L01_PrintDetails(spi_nrf24l01_t* pHandle);

/**
 * @brief Use this function to check if the radio's RX FIFO levels are all
 * occupied. This can be used to prevent data loss because any incoming
 * transmissions are rejected if there is no unoccupied levels in the RX
 * FIFO to store the incoming payload. Remember that each level can hold
 * up to a maximum of 32 bytes.
 */
bool NRF24L01_RxFifoFull(spi_nrf24l01_t* pHandle);

/**
 * @param[in] bFifoSel true focuses on the TX FIFO, and false focuses on the RX FIFO
 * @retval 0 if the specified FIFO is neither full nor empty.
 * @retval 1 if the specified FIFO is empty.
 * @retval 2 if the specified FIFO is full.
 */
uint8_t NRF24L01_GetFifoStatus(spi_nrf24l01_t* pHandle, bool bFifoSel);

/**
 * @brief This function is used to configure what events will trigger the Interrupt
 * Request (IRQ) pin active LOW.
 * The following events can be configured:
 * 1. "data sent": This does not mean that the data transmitted was
 * received, only that the attempt to send it was complete.
 * 2. "data failed": This means the data being sent was not received. This
 * event is only triggered when the auto-ack feature is enabled.
 * 3. "data received": This means that data from a receiving payload has
 * been loaded into the RX FIFO buffers. Remember that there are only 3
 * levels available in the RX FIFO buffers.
 *
 * By default, all events are configured to trigger the IRQ pin active LOW.
 * When the IRQ pin is active, use NRF24L01_WhatHappened() to determine what events
 * triggered it. Remember that calling NRF24L01_WhatHappened() also clears these
 * events' status, and the IRQ pin will then be reset to inactive HIGH.
 *
 * The following code configures the IRQ pin to only reflect the "data received" event:
 * @code
 * radio.maskIRQ(1, 1, 0);
 * @endcode
 *
 * @param[in] bTxOk  `true` ignores the "data sent" event, `false` reflects the
 * "data sent" event on the IRQ pin.
 * @param[in] bTxFail  `true` ignores the "data failed" event, `false` reflects the
 * "data failed" event on the IRQ pin.
 * @param[in] bRxRdy `true` ignores the "data received" event, `false` reflects the
 * "data received" event on the IRQ pin.
 */
void NRF24L01_MaskIRQ(spi_nrf24l01_t* pHandle, bool bTxOk, bool bTxFail, bool bRxRdy);

/**
 * @brief Set the address width from 3 to 5 bytes (24, 32 or 40 bit)
 *
 * @param[in] u8AddrWidth The address width (in bytes) to use; this can be 3, 4 or 5.
 */
void NRF24L01_SetAddressWidth(spi_nrf24l01_t* pHandle, uint8_t u8AddrWidth);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void NRF24L01_Test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
