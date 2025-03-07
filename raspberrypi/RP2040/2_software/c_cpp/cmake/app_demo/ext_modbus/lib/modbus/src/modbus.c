#include "mbcom.h"

uint16_t mb_crc16(const uint8_t* src, uint8_t len)
{
    uint16_t crc = 0xFFFF;
    size_t   i, j;

    for (i = 0; i < len; i++) {
        crc ^= (uint16_t)src[i];

        for (j = 0; j < 8; j++) {
            if (crc & 0x0001UL) {
                crc = (crc >> 1U) ^ 0xA001;
            } else {
                crc = crc >> 1U;
            }
        }
    }

    return __builtin_bswap16(crc);
}