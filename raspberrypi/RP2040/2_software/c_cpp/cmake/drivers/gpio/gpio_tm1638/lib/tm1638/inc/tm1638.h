// TM1638 module(LED & KEY)

#ifndef TM1638_H
#define TM1638_H

#include "pico/stdlib.h"
#include <cstdio>
#include <cstring>
#include "tm1638drv.h"

class TM1638 : public TM1638_DRV {
public:
    // Constructor
    TM1638(uint8_t strobe, uint8_t clock, uint8_t data);

    // Methods
    uint8_t readButtons(void);

    void setLEDs(uint16_t greenred);
    void setLED(uint8_t position, uint8_t value);

    void displayText(const char* text);
    void displayASCII(uint8_t position, uint8_t ascii);
    void displayASCIIwDot(uint8_t position, uint8_t ascii);
    void displayHex(uint8_t position, uint8_t hex);
    void display7Seg(uint8_t position, uint8_t value);
    void displayIntNum(unsigned long number, bool leadingZeros = true, AlignTextType_e = TMAlignTextLeft);
    void DisplayDecNumNibble(uint16_t numberUpper, uint16_t numberLower, bool leadingZeros = true, AlignTextType_e = TMAlignTextLeft);
};

#endif
