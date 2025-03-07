使用 6 根线将 k210 与 esp32 进行连接（spi）：

| pin        | esp32 | k210            |
| ---------- | ----- | --------------- |
| spi_mosi   | 14    | 28（SPI1_D0）   |
| spi_sclk   | 18    | 27（SPI1_SCLK） |
| spi_miso   | 23    | 26（SPI1_D1）   |
| spi_cs     | 5     | 25              |
| wifi_rst   | en    | 8               |
| wifi_ready | 25    | 9               |

![maixduino_pin](\image\maixduino_pin.png)

TCP 服务器：

![tcpserver](E:\how-to-use-kendryte-k210\3.MaixPy\4.Learning\16.wifi_tcpclient (esp32)\image\tcpserver.png)