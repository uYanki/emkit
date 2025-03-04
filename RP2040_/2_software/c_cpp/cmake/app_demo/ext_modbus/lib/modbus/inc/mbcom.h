
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define MBRTU_FRAME_MAXSIZEE 256
#define MB_MAX_REGISTERS     64  // 125

enum mb_state {
    MB_DATA_READY,
    MB_DATA_INCOMPLETE,
    MB_ERROR,
    MB_INVALID_SERVER_ADDRESS,
    MB_INVALID_FUNCTION,
};

typedef enum {

    // salve errors
    MB_NO_ERROR                                       = 0x00,
    MB_ERROR_ILLEGAL_FUNCTION                         = 0x01,
    MB_ERROR_ILLEGAL_DATA_ADDRESS                     = 0x02,
    MB_ERROR_ILLEGAL_DATA_VALUE                       = 0x03,
    MB_ERROR_SERVER_DEVICE_FAILURE                    = 0x04,
    MB_ERROR_ACKNOWLEDGE                              = 0x05,
    MB_ERROR_SERVER_DEVICE_BUSY                       = 0x06,
    MB_ERROR_NEGATIVE_ACKNOWLEDGE                     = 0x07,
    MB_ERROR_MEMORY_PARITY                            = 0x08,
    MB_ERROR_GATEWAY_PATH_UNAVAILABLE                 = 0x0A,
    MB_ERROR_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPONSE = 0x0B,

    // master errors
    MB_ERROR_TIMEOUT = 0xF0,
    MB_ERROR_INVALID_CRC,
    MB_ERROR_UNEXPECTED_RESPONSE,

} mb_result_e;

typedef enum {
    MB_READ_COIL_STATUS         = 0x01,
    MB_READ_INPUT_STATUS        = 0x02,
    MB_READ_HOLDING_REGISTERS   = 0x03,
    MB_READ_INPUT_REGISTERS     = 0x04,
    MB_WRITE_SINGLE_COIL        = 0x05,
    MB_WRITE_SINGLE_REGISTER    = 0x06,
    MB_WRITE_MULTIPLE_COILS     = 0x0F,
    MB_WRITE_MULTIPLE_REGISTERS = 0x10,
} mb_function_e;

typedef struct __attribute((packed)) {
    uint8_t address;
    uint8_t function;
    uint8_t data[MBRTU_FRAME_MAXSIZEE - 2];
} mbrtu_frame_t;

uint16_t mb_crc16(const uint8_t* buf, uint8_t len);
