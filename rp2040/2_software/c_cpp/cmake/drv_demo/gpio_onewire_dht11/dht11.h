#ifndef DHT11__H
#define DHT11__H

#include "pico/stdlib.h"

class DHT11 {
    ///< @brief Threshold for differentiating between bit 0 and bit 1 during DHT11 data transmission.
    const uint16_t THRESHOLD = 7;

    ///< @brief Maximum number of polling attempts during DHT11 data transmission.
    const uint16_t POLLING_LIMIT = 50;

    uint8_t m_pin;

public:
    DHT11(uint8_t pin);
    ~DHT11();
    bool read(void);

    float m_temperature = 0.f;
    float m_humidity    = 0.f;
};

#endif