/**
 * @brief tables for rapid CRC calculation
 */
#ifndef __CRC32_H__
#define __CRC32_H__

/*
 * Computes the CRC-CCITT, starting with an initialization value.
 * buf: the data on which to apply the checksum
 * length: the number of bytes of data in 'buf' to be calculated.
 */
unsigned short crc16(const unsigned char* buf, unsigned int length);

/*
 * Computes an updated version of the CRC-CCITT from existing CRC.
 * crc: the previous values of the CRC
 * buf: the data on which to apply the checksum
 * length: the number of bytes of data in 'buf' to be calculated.
 */
unsigned short update_crc16(unsigned short crc, const unsigned char* buf, unsigned int len);

unsigned long crc32(unsigned long crc, const unsigned char* buf, unsigned int len);

unsigned long adler32(unsigned long adler, const unsigned char* buf, unsigned int len);

#endif /* __CRC32_H__ */
