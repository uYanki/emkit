

#include <stdio.h>
#include <stdint.h>

#define CAN_SJW_1tq  ((uint8_t)0x00)
#define CAN_SJW_2tq  ((uint8_t)0x01)
#define CAN_SJW_3tq  ((uint8_t)0x02)
#define CAN_SJW_4tq  ((uint8_t)0x03)

#define CAN_BS1_1tq  ((uint8_t)0x00)
#define CAN_BS1_2tq  ((uint8_t)0x01)
#define CAN_BS1_3tq  ((uint8_t)0x02)
#define CAN_BS1_4tq  ((uint8_t)0x03)
#define CAN_BS1_5tq  ((uint8_t)0x04)
#define CAN_BS1_6tq  ((uint8_t)0x05)
#define CAN_BS1_7tq  ((uint8_t)0x06)
#define CAN_BS1_8tq  ((uint8_t)0x07)
#define CAN_BS1_9tq  ((uint8_t)0x08)
#define CAN_BS1_10tq ((uint8_t)0x09)
#define CAN_BS1_11tq ((uint8_t)0x0A)
#define CAN_BS1_12tq ((uint8_t)0x0B)
#define CAN_BS1_13tq ((uint8_t)0x0C)
#define CAN_BS1_14tq ((uint8_t)0x0D)
#define CAN_BS1_15tq ((uint8_t)0x0E)
#define CAN_BS1_16tq ((uint8_t)0x0F)

#define CAN_BS2_1tq  ((uint8_t)0x00)
#define CAN_BS2_2tq  ((uint8_t)0x01)
#define CAN_BS2_3tq  ((uint8_t)0x02)
#define CAN_BS2_4tq  ((uint8_t)0x03)
#define CAN_BS2_5tq  ((uint8_t)0x04)
#define CAN_BS2_6tq  ((uint8_t)0x05)
#define CAN_BS2_7tq  ((uint8_t)0x06)
#define CAN_BS2_8tq  ((uint8_t)0x07)

int main()
{
    uint32_t idx, sjw, bs1, bs2, psc;

    uint32_t bitrates[] = {1000e3, 800e3, 500e3, 250e3, 125e3, 100e3, 50e3, 20e3, 10e3};

    for (idx = 0; idx < (sizeof(bitrates) / sizeof(*bitrates)); ++idx)
    {
        for (psc = 1; psc <= 1024; ++psc)
        {
            for (sjw = CAN_SJW_1tq; sjw <= CAN_SJW_4tq; ++sjw)
            {
                for (bs1 = CAN_BS1_1tq; bs1 <= CAN_BS1_16tq; ++bs1)
                {
                    for (bs2 = CAN_BS2_1tq; bs2 <= CAN_BS2_8tq; ++bs2)
                    {
                        if (bs2 >= sjw)
                        {
                            uint32_t BTR = (sjw << 24) | (bs1 << 16) | (bs2 << 20) | (psc - 1);
                            uint32_t SJW = sjw + 1, BS1 = bs1 + 1, BS2 = bs2 + 1, Prescaler = psc;

                            uint32_t bitrate      = 42e6 / psc / (SJW + BS1 + BS2);
                            float    sample_point = (SJW + BS1) / (float)(SJW + BS1 + BS2) * 100;  // 采样点

                            if (bitrate == bitrates[idx] && sample_point > 80)
                            {
                                printf("0x%08X (sjw = %d, bs1 = %d, bs2 = %d, psc = %d, bitrate = %d, Sample Point = %.1f%%)\n",
                                       BTR, SJW, BS1, BS2, Prescaler, bitrate, sample_point);
                            }
                        }
                    }
                }
            }
        }
        printf("\n");
    }

    return 0;
}
