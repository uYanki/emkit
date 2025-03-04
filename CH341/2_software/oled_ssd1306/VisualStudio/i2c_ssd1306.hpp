#pragma once

#include "ch341.hpp"

// oled - ssd1306
class ssd1306 : protected ch341 {
public:
    ssd1306(uint8_t port = 0) : ch341(port) {}

    ~ssd1306() {}

    bool write_cmd(uint8_t cmd) { return ch341::i2c_tx(0x3C, 0x00, cmd); }

    bool write_data(uint8_t dat) { return ch341::i2c_tx(0x3C, 0x40, dat); }

    void write_ndata(uint8_t* data, uint16_t len)
    {
        uint16_t i;
        for (i = 0; i < len; ++i) {
            write_data(data[i]);
        }
    }

    void fill(uint8_t data)
    {
        static uint8_t zerobuff[128] = {0x00};

        uint8_t y;

        for (y = 0; y < 128; ++y) {
            zerobuff[y] = data;
        }
        for (y = 0; y < 8; ++y) {
            set_cursor(0, y);
            write_ndata((uint8_t*)zerobuff, 128);
        }
    }

    void init(void)
    {
        write_cmd(0xAE);  // display off
        write_cmd(0x20);  // Set Memory Addressing Mode
        write_cmd(0x10);  // 00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
        write_cmd(0xb0);  // Set Page Start Address for Page Addressing Mode,0-7
        write_cmd(0xc8);  // Set COM Output Scan Direction
        write_cmd(0x00);  // set low column address
        write_cmd(0x10);  // set high column address
        write_cmd(0x40);  // set start line address
        write_cmd(0x81);  // set contrast control register
        write_cmd(0xff);
        write_cmd(0xa1);  // set segment re-map 0 to 127
        write_cmd(0xa6);  // set normal display
        write_cmd(0xa8);  // set multiplex ratio(1 to 64)
        write_cmd(0x3F);
        write_cmd(0xa4);  // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
        write_cmd(0xd3);  // -set display offset
        write_cmd(0x00);  // -not offset
        write_cmd(0xd5);  // set display clock divide ratio/oscillator frequency
        write_cmd(0xf0);  // set divide ratio
        write_cmd(0xd9);  // set pre-charge period
        write_cmd(0x22);  //
        write_cmd(0xda);  // set com pins hardware configuration
        write_cmd(0x12);
        write_cmd(0xdb);  // set vcomh
        write_cmd(0x20);  // 0x20,0.77xVcc
        write_cmd(0x8d);  // set DC-DC enable
        write_cmd(0x14);
        write_cmd(0xaf);  // turn on SSD1306 panel
    }

    void set_cursor(uint8_t x, uint8_t y)
    {
        write_cmd(0xb0 + y);
        write_cmd(((x & 0xf0) >> 4) | 0x10);
        write_cmd(x & 0x0f);
    }

    void clear(void)
    {
        fill(0x00);
    }

    void display_on(void)
    {
        write_cmd(0X8D);
        write_cmd(0X14);
        write_cmd(0XAF);
    }

    void display_off(void)
    {
        write_cmd(0X8D);
        write_cmd(0X10);
        write_cmd(0XAE);
    }

    void fill_img(uint8_t img[1024])
    {
        set_cursor(0, 0);
        write_ndata(img, 1024);
    }
};
