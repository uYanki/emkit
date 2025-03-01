[STM32 下载烧录教程以及问题汇总](http://www.weact-tc.cn/2019/11/30/STM32Download/)

# SPI Flash Erase Firmware

> 当出现 U 盘无法识别的时候（py 代码编写错误引起），可以烧录该固件擦除 SPI Flash

由于 openmv 固件是没有办法擦除外部的 SPI 存储器，所以我们提供`SPI Flash Erase Firmware`固件用于擦除外部的两颗`SPI Flash`

- `SPI_Flash_Erase_0x8000000.hex`可以使用我们提供的`WeAct Studio USB Download Tool`进行下载，下载完成后，等待程序运行完成，核心板屏幕显示`Please Burn Next Fiwmware`后可以重新烧录固件
- `SPI_Flash_Erase_0x8040000.bin`可以直接使用 openmv IDE 进行下载，具体方法为：

1. 选择`工具->运行引导加载程序(加载固件)`
2. 选择`SPI_Flash_Erase_0x8040000.bin`,然后点击运行即可
3. 下载完成后，等待程序运行完成，核心板屏幕显示`Please Burn Next Fiwmware`后继续下一步
4. openmv IDE 继续勾选`工具->运行引导加载程序(加载固件)`,选择我们资料提供的`firmware.bin`固件(不是 USB Download 软件里面的固件)，然后点击运行,之后核心板单击复位按钮，等待固件下载完成

# OpenMV Firmware（V4.3.0）

## Internal Flash Firmware

> Openmv 固件放在内部地址，从 0x08000000 开始


> 8MB SPI Flash 作为 Python 代码存储器

### 固件下载地址定义

- bootloader.\* -> 0x08000000
- firmware.\* -> 0x08040000
- openmv.\* -> 0x08000000

openmv.\* 为 bootloader 和 firmware 两个固件的合并固件，初次烧录选择 openmv.bin 即可

> 固件下载需要使用 USB 模式下载，ST-Link 下载会有大小限制，可以使用`WeAct Studio USB Download Tool`或者使用 STM32CubeProg USB 模式进行下载，建议使用`WeAct Studio USB Download Tool`烧录 openmv.bin 文件。

## QSPI Flash Firmware

Openmv 固件放在外部8MB QSPI Flash，所以需要下载0x08000000.hex，初始化QSPI外设，将QSPI Flash映射到0x90000000。

### 固件下载地址定义

0x08000000.hex -> 0x08000000

0x90000000.bin -> 0x90000000

### 烧录方法

1. 使用`STM32CubeProg`或者`WeAct Studio USB Download Tool`烧录0x08000000.hex
2. 使用WeAct HID Flash烧录0x90000000.bin

### 0x08000000.hex 说明

1. 按住K1键上电或复位，LED慢闪时松开K1键进入WeAct HID Flash，可以使用WeAct HID Flash 上位机烧录openmv app: 0x90000000.bin
2. 按住K1键上电或复位，LED慢闪，继续按住K1键，LED变为快闪，松开K1键进入DFU模式