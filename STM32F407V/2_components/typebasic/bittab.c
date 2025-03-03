#include "bittab.h"

#define BITS_PER_BYTE (sizeof(u8) * 8)

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "bitops"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

u8 revbit8(u8 u8Data)
{
#if CONFIG_REVBIT_TABLE_SW

    // clang-format off
    static const u8 aTable[] = {
        0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0, 0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
        0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4, 0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
        0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2, 0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
        0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6, 0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
        0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1, 0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
        0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5, 0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
        0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3, 0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
        0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7, 0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
    };
    // clang-format on

    return aTable[u8Data];

#else

    u8 x = u8Data;

    x = ((x >> 1) & 0x55) | ((x & 0x55) << 1);
    x = ((x >> 2) & 0x33) | ((x & 0x33) << 2);
    return (x >> 4) | (x << 4);

#endif
}

u16 revbit16(u16 u16Data)
{
#if CONFIG_REVBIT_TABLE_SW

    return ((u16)revbit8(u16Data) << 8) | (u16)revbit8((u16Data >> 8));

#else

    u16 x = u16Data;

    x = ((x >> 1) & 0x5555) | ((x & 0x5555) << 1);
    x = ((x >> 2) & 0x3333) | ((x & 0x3333) << 2);
    x = ((x >> 4) & 0x0f0f) | ((x & 0x0f0f) << 4);
    return ((x >> 8) | (x << 8));

#endif
}

u32 revbit32(u32 u32Data)
{
#if CONFIG_REVBIT_TABLE_SW

    return ((u32)revbit16(u32Data) << 16) | (u32)revbit16((u32Data >> 16));

#else

    u32 x = u32Data;

    x = (((x & 0xAAAAAAAA) >> 1) | ((x & 0x55555555) << 1));
    x = (((x & 0xCCCCCCCC) >> 2) | ((x & 0x33333333) << 2));
    x = (((x & 0xF0F0F0F0) >> 4) | ((x & 0x0F0F0F0F) << 4));
    x = (((x & 0xFF00FF00) >> 8) | ((x & 0x00FF00FF) << 8));

    return ((x >> 16) | (x << 16));

#endif
}

void setbits(u8* pu8Data, u16 u16StartBit, u8 u8BitWidth, u8 u8Value)
{
    u16 u16Word;
    u16 u16Mask;
    u16 u18ByteOffset;
    u16 u16NPreBits;
    u16 u16Value = u8Value;

    // assert(u8BitWidth <= 8);
    // assert(BITS_PER_BYTE == sizeof(u8) * 8);

    /* Calculate byte offset for first byte containing the bit values starting at u16BitOffset. */
    u18ByteOffset = (u16)((u16StartBit) / BITS_PER_BYTE);

    /* How many bits precede our bits to set. */
    u16NPreBits = (u16)(u16StartBit - u18ByteOffset * BITS_PER_BYTE);

    /* Move bit field into position over bits to set */
    u16Value <<= u16NPreBits;

    /* Prepare a mask for setting the new bits. */
    u16Mask = (u16)((1 << (u16)u8BitWidth) - 1);
    u16Mask <<= u16StartBit - u18ByteOffset * BITS_PER_BYTE;

    /* copy bits into temporary storage. */
    u16Word = pu8Data[u18ByteOffset];
    u16Word |= pu8Data[u18ByteOffset + 1] << BITS_PER_BYTE;

    /* Zero out bit field bits and then or value bits into them. */
    u16Word = (u16)((u16Word & (~u16Mask)) | u16Value);

    /* move bits back into storage */
    pu8Data[u18ByteOffset]     = (u8)(u16Word & 0xFF);
    pu8Data[u18ByteOffset + 1] = (u8)(u16Word >> BITS_PER_BYTE);
}

u8 getbits(u8* pu8Data, u16 u16StartBit, u8 u8BitWidth)
{
    u16 u16Word;
    u16 u16Mask;
    u16 u16ByteOffset;
    u16 u16NPreBits;

    /* Calculate byte offset for first byte containing the bit values starting at u16BitOffset. */
    u16ByteOffset = (u16)((u16StartBit) / BITS_PER_BYTE);

    /* How many bits precede our bits to set. */
    u16NPreBits = (u16)(u16StartBit - u16ByteOffset * BITS_PER_BYTE);

    /* Prepare a mask for setting the new bits. */
    u16Mask = (u16)((1 << (u16)u8BitWidth) - 1);

    /* copy bits into temporary storage. */
    u16Word = pu8Data[u16ByteOffset];
    u16Word |= pu8Data[u16ByteOffset + 1] << BITS_PER_BYTE;

    /* throw away unneeded bits. */
    u16Word >>= u16NPreBits;

    /* mask away bits above the requested bitfield. */
    u16Word &= u16Mask;

    return (u8)u16Word;
}
