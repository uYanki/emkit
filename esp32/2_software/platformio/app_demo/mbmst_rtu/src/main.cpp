#include <Arduino.h>
#include "ModbusMaster.h"

ModbusMaster node;

void setup()
{
    Serial.begin(115200);

    Serial1.begin(19200, SERIAL_8E1);
    Serial1.setPins(4, 5);
    node.begin(1, Serial1);
}
void loop()
{
    delay(1000);

    uint8_t result;

    {
        // Read 30002, 30003, 30004

        result = node.readInputRegisters(2, 3);

        if (result == node.ku8MBSuccess)  // Read ok
        {
            uint16_t data[3];
            data[0] = node.getResponseBuffer(0);
            data[1] = node.getResponseBuffer(1);
            data[2] = node.getResponseBuffer(2);
            Serial.printf("Value 30001: %d\r\n", data[0]);
            Serial.printf("Value 30002: %d\r\n", data[1]);
            Serial.printf("Value 30003: %d\r\n", data[2]);
        }
        else
        {
            Serial.print("Read input fail\r\n");
        }
    }

    {
        // Read 40001, 40002, 40003

        result = node.readHoldingRegisters(1, 3);

        if (result == node.ku8MBSuccess)  // Read ok
        {
            uint16_t data[3];
            data[0] = node.getResponseBuffer(0);
            data[1] = node.getResponseBuffer(1);
            data[2] = node.getResponseBuffer(2);
            Serial.printf("Value 40001: %d\r\n", data[0]);
            Serial.printf("Value 40002: %d\r\n", data[1]);
            Serial.printf("Value 40003: %d\r\n", data[2]);
        }
        else
        {
            Serial.print("Read holding fail\r\n");
        }
    }

    {
        // FC06, write 40001 value 5

        result = node.writeSingleRegister(1, 5);

        if (result == node.ku8MBSuccess)
        {
            Serial.printf("Write Success\r\n");
        }
        else
        {
            Serial.printf("Write Fail\r\n");
        }
    }
}
