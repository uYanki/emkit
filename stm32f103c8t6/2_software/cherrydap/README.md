# [DAPLink Over CherryUSB](https://kkgithub.com/HaiMianBBao/CherryUSB-STM32F1-DAPLink)

[CherryUSB](https://kkgithub.com/cherry-embedded/CherryUSB) 版本：使用 CherryUSB v1.3.1 无法通过编译，使用 CherryUSB-0.7.0 编译成功

## Pins

| Function  | Label | GPIO |
| :-------- | :---: | :--: |
| JTAG_TDI  |  TDI  |  A1  |
| JTAG_TDO  |  TDO  |  A5  |
| JTAG_TMS  |  TMS  |  A6  |
| JTAG_TCK  |  TCK  |  A4  |
| SWD_SWDIO |  TMS  |  A6  |
| SWD_SWCLK |  TCK  |  A4  |
| nRESET    |  RTS  |  A0  |
| UART TX   |  TX   |  A9  |
| UART RX   |  RX   | A10  |

RESET 引脚必接，不接大概率烧录失败！！

