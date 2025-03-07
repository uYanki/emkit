/*!
    TESTS:
    00. TEST 0 Reset
    01. TEST 1 Brightness
    02. TEST 2 ASCII display
    03. TEST 3 Set a single segment
    04. TEST 4 Hex digits
    05. TEST 5 Text String with Decimal point
    06. TEST 6 TEXT + ASCII combo
    07. TEST 7 Integer Decimal number
    08. TEST 8 Text String + Float
    09. TEST 9 Text String + decimal number
    10. TEST 10 Multiple dots
    11. TEST 11 Display Overflow
    12. TEST 12 Scrolling text
    13. TEST 13 setLED and setLEDs method
    14. TEST 14 Buttons + LEDS
*/

#include "pico/stdlib.h"
#include "tm1638.h"  //include the library

// GPIO I/O pins on the  PICO connected to strobe, clock, data,
// pick on any I/O you want.
#define STROBE_TM 2  // strobe
#define CLOCK_TM  3  // clock
#define DIO_TM    4  // data

// Constructor object (GPIO STB , GPIO CLOCK , GPIO DIO)
TM1638 tm(STROBE_TM, CLOCK_TM, DIO_TM);

// Some vars and defines for the tests.
#define myTestDelay  1500
#define myTestDelay3 1000
#define myTestDelay1 500
uint8_t testcount = 0;

void Test0(void);
void Test1(void);
void Test2(void);
void Test3(void);
void Test4(void);
void Test5(void);
void Test6(void);
void Test7(void);
void Test8(void);
void Test9(void);
void Test10(void);
void Test11(void);
void Test12(void);
void Test13(void);
void Test14(void);
void doLEDs(uint8_t value);

int main()
{
    tm.displayBegin();
    busy_wait_ms(myTestDelay1);
    // Test 0 reset
    Test0();
    while (true) {
        switch (testcount) {
            case 1: Test1(); break;    // Brightness
            case 2: Test2(); break;    // ASCII display
            case 3: Test3(); break;    // Set a single segment
            case 4: Test4(); break;    // Hex digits
            case 5: Test5(); break;    // Text String with Decimal point
            case 6: Test6(); break;    // TEXT + ASCII combo
            case 7: Test7(); break;    // Integer Decimal number
            case 8: Test8(); break;    // Text String + Float hack
            case 9: Test9(); break;    // Text String + decimal number
            case 10: Test10(); break;  // Multiple Decimal points
            case 11: Test11(); break;  // Display Overflow
            case 12: Test12(); break;  // Scrolling text
            case 13: Test13(); break;  // setLED and setLEDs
            case 14: Test14(); break;  // Buttons + LEDS stays here forever
        }
        testcount++;
    }
}

void Test0()
{
    // Test 0 reset test
    tm.setLED(0, 1);
    busy_wait_ms(myTestDelay);
    tm.reset();
}

void Test1()
{
    // Test 1  Brightness and reset
    for (uint8_t brightness = 0; brightness < 8; brightness++) {
        tm.brightness(brightness);
        tm.displayText("00000000");
        busy_wait_ms(myTestDelay1);
    }
    tm.reset();
    // restore default brightness
    tm.brightness(0x02);
}

void Test2()
{
    // Test 2 ASCII , display 2.341

    tm.displayASCIIwDot(0, '2');
    tm.displayASCII(1, '3');
    tm.displayASCII(2, '4');
    tm.displayASCII(3, '1');
    busy_wait_ms(myTestDelay);
    tm.reset();
}

void Test3()
{
    // TEST 3 single segment (digit position, (dp)gfedcba)
    //  (dp)gfedcba =  seven segments positions
    uint8_t pos = 0;
    for (pos = 0; pos < 8; pos++) {
        tm.display7Seg(pos, 1 << 7 - pos);  // Displays a single seg in (dp)gfedcba) in each  pos 0-7
        busy_wait_ms(myTestDelay1);
    }
}

void Test4()
{
    // Test 4 Hex digits.
    tm.displayHex(0, 0);
    tm.displayHex(1, 1);
    tm.displayHex(2, 2);
    tm.displayHex(3, 3);
    tm.displayHex(4, 4);
    tm.displayHex(5, 5);
    tm.displayHex(6, 6);
    tm.displayHex(7, 7);
    busy_wait_ms(myTestDelay);  // display 12345678

    tm.displayHex(0, 8);
    tm.displayHex(1, 9);
    tm.displayHex(2, 0x0A);
    tm.displayHex(3, 0x0B);
    tm.displayHex(4, 0x0C);
    tm.displayHex(5, 0x0D);
    tm.displayHex(6, 0x0E);
    tm.displayHex(7, 0x0F);
    busy_wait_ms(myTestDelay);  // display 89ABCDEF
    tm.reset();

    tm.displayHex(1, 0xFE);
    tm.displayHex(7, 0x10);
    busy_wait_ms(myTestDelay);  // display " E      0"
}

void Test5()
{
    // Test 5 TEXT  with dec point
    // abcdefgh with decimal point for c and d
    tm.displayText("abc.d.efgh");
    busy_wait_ms(myTestDelay);
}

void Test6()
{
    // Test6  TEXT + ASCII combo
    // ADC=.2.948
    char text1[] = "ADC=.";
    tm.displayText(text1);
    tm.displayASCIIwDot(4, '2');
    tm.displayASCII(5, '9');
    tm.displayASCII(6, '4');
    tm.displayASCII(7, '8');
    busy_wait_ms(myTestDelay);
    tm.reset();
}

void Test7()
{
    // TEST 7a Integer left aligned , NO leading zeros
    tm.displayIntNum(45, false, TMAlignTextLeft);  // "45      "
    busy_wait_ms(myTestDelay);
    // TEST 7b Integer left aligned , leading zeros
    tm.displayIntNum(99991, true, TMAlignTextLeft);  // "00099991"
    busy_wait_ms(myTestDelay);
    tm.reset();
    // TEST 7c Integer right aligned , NO leading zeros
    tm.displayIntNum(35, false, TMAlignTextRight);  // "      35"
    busy_wait_ms(myTestDelay);
    // TEST 7d Integer right aligned , leading zeros
    tm.displayIntNum(9983551, true, TMAlignTextRight);  // "09983551"
    busy_wait_ms(myTestDelay);

    // TEST 7e tm.DisplayDecNumNIbble left aligned
    tm.DisplayDecNumNibble(134, 70, false, TMAlignTextLeft);  // "134 " "70" , left aligned, NO leading zeros
    busy_wait_ms(myTestDelay);
    tm.DisplayDecNumNibble(23, 662, true, TMAlignTextLeft);  // "0023" "0662" , left aligned , leading zeros
    busy_wait_ms(myTestDelay);
    tm.reset();

    // TEST 7f tm.DisplayDecNumNIbble right aligned
    tm.DisplayDecNumNibble(43, 991, false, TMAlignTextRight);  // "  43" " 991" , right aligned, NO leading zeros
    busy_wait_ms(myTestDelay);
    tm.DisplayDecNumNibble(53, 8, true, TMAlignTextRight);  // "0053" "0008" , right aligned , leading zeros
    busy_wait_ms(myTestDelay);
}

void Test8()
{
    // TEST 8  TEXT STRING + integer SSSSIIII
    char     workStr[11];
    uint16_t data = 234;
    sprintf(workStr, "ADC=.%04d", data);  // "ADC=.0234"
    tm.displayText(workStr);
    busy_wait_ms(myTestDelay);
}

void Test9()
{
    // TEST 9 Text String + Float  SSSSFFFF ,  just one possible method.
    float    voltage = 12.45;
    uint16_t temp    = 0;
    char     workStr[11];
    uint8_t  digit1, digit2, digit3, digit4;
    voltage = voltage * 100;  // 1245
    temp    = (uint16_t)voltage;
    digit1  = (temp / 1000) % 10;
    digit2  = (temp / 100) % 10;
    digit3  = (temp / 10) % 10;
    digit4  = temp % 10;

    sprintf(workStr, "ADC=.%d%d.%d%d", digit1, digit2, digit3, digit4);
    tm.displayText(workStr);  // 12.45.VOLT
    busy_wait_ms(myTestDelay);
    tm.reset();
}

void Test10()
{
    // TEST 10 Multiple dots test
    tm.displayText("Hello...");
    busy_wait_ms(myTestDelay);
    tm.displayText("...---...");  // SOS in morse
    busy_wait_ms(myTestDelay);
}

void Test11()
{
    // TEST11 user overflow
    tm.displayText("1234567890abc");  // should display just 12345678
    busy_wait_ms(myTestDelay);
    tm.reset();
}

void Test12()
{
    // TEST 12 scrolling text, just one possible method.
    char          textScroll[17]         = " Hello world 123";
    unsigned long previousMillis_display = 0;     // will store last time display was updated
    const long    interval_display       = 1000;  //   interval at which to update display (milliseconds)

    while (1) {
        tm.displayText(textScroll);
        unsigned long currentMillis = to_ms_since_boot(get_absolute_time());

        if (currentMillis - previousMillis_display >= interval_display) {
            previousMillis_display = currentMillis;
            if (strlen(textScroll) > 0) {
                memmove(textScroll, textScroll + 1, strlen(textScroll));
                tm.displayText("        ");  // Clear display or last character will drag across screen
            } else {
                return;
            }
        }
    }
}

void Test13()
{
    // Test 13 LED display
    uint8_t LEDposition = 0;

    // Test 13A Turn on redleds one by one, left to right, with setLED where 0 is L1 and 7 is L8 (L8 RHS of display)
    for (LEDposition = 0; LEDposition < 8; LEDposition++) {
        tm.setLED(LEDposition, 1);
        busy_wait_ms(500);
        tm.setLED(LEDposition, 0);
    }

    // TEST 13b test setLEDs function (0xLEDXX) ( L8-L1 , XX )
    // NOTE passed L8-L1 and on display L8 is on right hand side. i.e. 0x01 turns on L1. LXXX XXXX
    // For model 1 just use upper byte , lower byte is is used by model3 for bi-color leds leave at 0x00 for model 1.
    tm.setLEDs(0xFF00);  //  all LEDs on
    busy_wait_ms(myTestDelay3);
    tm.setLEDs(0x0100);  // Displays as LXXX XXXX (L1-L8) , NOTE on display L8 is on right hand side.
    busy_wait_ms(myTestDelay3);
    tm.setLEDs(0xF000);  //  Displays as XXXX LLLL (L1-L8) , NOTE on display L8 is on right hand side.
    busy_wait_ms(myTestDelay3);
    tm.setLEDs(0x0000);  // all off
    busy_wait_ms(myTestDelay3);
}

void Test14()
{
    // Test 14 buttons and LED test, press switch number S-X to turn on LED-X, where x is 1-8.
    // The HEx value of switch is also sent to Serial port.
    tm.displayText("buttons ");
    busy_wait_ms(2000);
    while (1)  // Loop here forever
    {
        uint8_t buttons = tm.readButtons();
        /* buttons contains a byte with values of button s8s7s6s5s4s3s2s1
         HEX  :  Switch no : Binary
         0x01 : S1 Pressed  0000 0001
         0x02 : S2 Pressed  0000 0010
         0x04 : S3 Pressed  0000 0100
         0x08 : S4 Pressed  0000 1000
         0x10 : S5 Pressed  0001 0000
         0x20 : S6 Pressed  0010 0000
         0x40 : S7 Pressed  0100 0000
         0x80 : S8 Pressed  1000 0000
        */
        doLEDs(buttons);
        tm.displayIntNum(buttons, true);
        busy_wait_ms(250);
    }
}

// scans the individual bits of value sets a LED based on which button pressed
void doLEDs(uint8_t value)
{
    for (uint8_t LEDposition = 0; LEDposition < 8; LEDposition++) {
        tm.setLED(LEDposition, value & 1);
        value = value >> 1;
    }
}
