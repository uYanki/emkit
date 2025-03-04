#include "Arduino.h"

#define RS485_DR         4
#define RS485_TX         16
#define RS485_RX         17

#define RS485_SetTxDir() digitalWrite(RS485_DR, HIGH)
#define RS485_SetRxDir() digitalWrite(RS485_DR, LOW)

HardwareSerial rs485(2);

void setup()
{
    rs485.begin(9600, SERIAL_8N1, RS485_RX, RS485_TX);
    pinMode(RS485_DR, OUTPUT);
    RS485_SetRxDir();
    delay(200);
}

void loop()
{
#if 0

    RS485_SetTxDir();
    delay(100);
    rs485.print("tx");
    delay(100);
    RS485_SetRxDir();
    delay(100);
    rs485.print("rx");
    delay(100);

#else

    if (rs485.available())
    {
        static char buff[255];

        int n   = rs485.readBytesUntil('\n', buff, sizeof(buff) - 1);
        buff[n] = '\0';

        // 单字节耗时 = 1s/(baudrate/10) = 1000000/(9600/10) = 1.0416 ms

        RS485_SetTxDir();
        delay(10);

        rs485.print(buff);

        delay(10);
        RS485_SetRxDir();
    }

#endif
}