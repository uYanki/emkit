#include "hexdump.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "hexdump"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

#ifndef nullptr
#define nullptr  (void*)0
#endif

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static inline bool isprint(char ch)
{
    return (ch > 0x1F && ch < 0x7F);
}

/**
 * @brief Dump a block of data as hexadecimal and ASCII to the provided stream.
 *
 * @param[in] cpu8Buffer        Pointer to block of data to dump.
 * @param[in] u32BytesDumped   Number of bytes in the @param cpu8Buffer.
 * @param[in] u8BytesPerLine   Number of bytes to print per line ( 16, 32; default = 16 ).
 * @param[in] u8BytesOncePrint Number of bytes to print at a time (1, 2, 4, 8; default = 1).
 * @param[in] bShowAscii       Include ASCII after the hex output.
 * @param[in] szPrefixFormat   The prefix format displayed in the left address column. default is "0x%08X: "
 * @param[in] u32ShownStartingAddress  The starting address displayed in the left address column.
 *
 * @return bool
 * @retval true: no remaining bytes
 * @retval false: return false if there are any remaining bytes
 *
 */
bool hexdump(const uint8_t* cpu8Buffer, uint32_t u32BytesDumped, uint8_t u8BytesPerLine, uint8_t u8BytesOncePrint, bool bShowAscii, const char* szPrefixFormat, uint32_t u32ShownStartingAddress)
{
    // current dumped byte pointer
    const char* pCurByteDumped = (const char*)cpu8Buffer;

    // line buffer to be output
    char aLineBuf[144 + 1] = {0};

    if (u8BytesPerLine != 16 && u8BytesPerLine != 32)
    {
        u8BytesPerLine = 16;
    }

    if (u8BytesOncePrint != 1 && u8BytesOncePrint != 2 && u8BytesOncePrint != 4 && u8BytesOncePrint != 8)
    {
        u8BytesOncePrint = 1;
    }

    if (szPrefixFormat == nullptr)
    {
        szPrefixFormat = "0x%08X: ";
    }

    while (u32BytesDumped >= (uint32_t)u8BytesOncePrint)
    {
        // current position of the line being written
        char* pLinePos = &aLineBuf[0];

        // number of bytes will be dumped in this line
        uint8_t u8LineBytesDumped;

        // number of bytes have been dumped in this line
        uint8_t u8LineBytesIndex = 0;

        u8LineBytesDumped = (u32BytesDumped < (uint32_t)u8BytesPerLine) ? (uint8_t)u32BytesDumped : u8BytesPerLine;
        u8LineBytesDumped -= u8LineBytesDumped % u8BytesOncePrint;

        pLinePos += sprintf(pLinePos, szPrefixFormat, u32ShownStartingAddress);

        while (u8LineBytesIndex < u8LineBytesDumped)
        {
            // clang-format off
            switch (u8BytesOncePrint)
            {
                case 1: pLinePos += sprintf(pLinePos, "%2.2X",     *(uint8_t*) &pCurByteDumped[u8LineBytesIndex]); break;
                case 2: pLinePos += sprintf(pLinePos, "%4.4X",     *(uint16_t*)&pCurByteDumped[u8LineBytesIndex]); break;
                case 4: pLinePos += sprintf(pLinePos, "%8.8X",     *(uint32_t*)&pCurByteDumped[u8LineBytesIndex]); break;
                case 8: pLinePos += sprintf(pLinePos, "%16.16llX", *(uint64_t*)&pCurByteDumped[u8LineBytesIndex]); break;
            }
            // clang-format on

            pLinePos += sprintf(pLinePos, " ");
            u8LineBytesIndex += u8BytesOncePrint;
        }

        while (u8LineBytesIndex < u8BytesPerLine)
        {
            // clang-format off
            switch (u8BytesOncePrint)
            {
                case 1: pLinePos += sprintf(pLinePos, "  ");               break;
                case 2: pLinePos += sprintf(pLinePos, "    ");             break;
                case 4: pLinePos += sprintf(pLinePos, "        ");         break;
                case 8: pLinePos += sprintf(pLinePos, "                "); break;
            }
            // clang-format on
            pLinePos += sprintf(pLinePos, " ");
            u8LineBytesIndex += u8BytesOncePrint;
        }

        if (bShowAscii)
        {
            pLinePos += sprintf(pLinePos, " |");

            u8LineBytesIndex = 0;

            while (u8LineBytesIndex < u8LineBytesDumped)
            {
                if (isprint(pCurByteDumped[u8LineBytesIndex]))
                {
                    pLinePos += sprintf(pLinePos, "%c", pCurByteDumped[u8LineBytesIndex]);
                }
                else
                {
                    pLinePos += sprintf(pLinePos, ".");
                }

                u8LineBytesIndex++;
            }

            pLinePos += sprintf(pLinePos, "|");
        }

#if defined(LOG_IMPL)
        LOG_IMPL("%s", aLineBuf);
#else
#error "hexdump is not effective"
#endif

        u32BytesDumped -= (uint32_t)u8LineBytesDumped;
        pCurByteDumped += u8LineBytesDumped;
        u32ShownStartingAddress += u8BytesPerLine;
    };

    // return false if there are any remaining bytes
    return u32BytesDumped == 0 ? true : false;
}
