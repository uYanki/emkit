http://arduino.luatos.com/airisp/

# AirISP 烧录工具

AirISP 是一个通过串口与芯片ISP功能进行交互，从而实现擦除、烧录、修改读保护等功能的一个小工具。

```
.\AirISP -h
Description:
  AirISP 是一个flash烧录工具

Usage:
  AirISP [command] [options]

Options:
  -c, --chip <chip>                      目标芯片型号，auto/air001
  -p, --port <port>                      串口名称
  -b, --baud <baud>                      串口波特率
  -t, --trace                            启用trace日志输出 [default: False]
  --connect-attempts <connect-attempts>  最大重试次数，小于等于0表示无限次，默认为10次 [default: 10]
  --before <before>                      下载前要执行的操作 [default: default_reset]
  --after <after>                        下载后要执行的操作 [default: hard_reset]
  --version                              Show version information
  -?, -h, --help                         Show help and usage information

Commands:
  chip_id                           获取芯片ID
  get                               获取ISP版本和支持的命令列表
  get_version                       获取ISP版本和芯片读保护状态
  write_flash <address> <filename>  向flash刷入固件
  read_unprotect                    关闭读保护
  read_protect                      开启读保护
```

# 刷写固件

AirISP支持烧录`HEX`或`BIN`格式的文件到芯片的 FLASH 中。

我们可以使用`write_flash`命令来执行烧录操作，像下面这样：



```bash
> .\AirISP.exe -c air001 -p COM21 -b 115200 write_flash -e 0x08000000 gpio.hex
AirISP v1.2.4.0
串口 COM21
连接中...
Chip PID is: 0x04 0x40
擦除flash中（请耐心等待）...
擦除成功，耗时 39.5811 ms.
Writing at 134219264... 100.00%
Write 1536 bytes at 0x08000000 in 274.0526 ms

Leaving...
通过RTS硬件复位...
```

## write_flash 命令参数

`write_flash`命令有如下参数：

1. `--erase-all`或者`-e`，作用是在烧录的时候擦除全部flash，建议添加。
2. `--no-progress`或者`-p`，作用是在下载的时候禁止显示进度条。

提示

若MCU中已刷入过其他固件，烧录新固件时请务必加上`-e`擦除参数。

提示

如若使用不带外置晶振的USB转串口芯片导致通信失败，可能需要降低波特率再试，比如`9600`。

# 读写保护

## 解除读保护

有时候我们会不小心将 MCU 的读保护打开，导致无法刷入固件，这是就需要去除读保护。

我们可以使用`read_unprotect`命令来关闭 FLASH 的读保护，像下面这样：



```bash
> .\AirISP.exe -c air001 -p COM21 -b 115200 read_unprotect
AirISP v1.2.4.0
串口 COM21
连接中...
Leaving...
通过RTS硬件复位...
```

注意

解除读保护的操作会导致 FLASH 数据被全部擦除。

## 启用读保护

我们可以使用`read_protect`命令来开启 FLASH 的读保护，像下面这样：

```bash
> .\AirISP.exe -c air001 -p COM21 -b 115200 read_protect
AirISP v1.2.4.0
串口 COM21
连接中...
Leaving...
通过RTS硬件复位...
```

注意

开启读保护后，会导致无法使用 ISP 工具进行烧录操作，需要解除读保护后才可操作。

# 基础参数

在获取`AiISP`工具后，我们就可以简单测试工具了，需要注意这几个基础参数。

## -c, --chip <目标芯片型号>

该参数用来指示当前的芯片型号，填上支持的芯片型号或者使用`auto`自动识别芯片。目前支持以下芯片：

| 芯片名称  | 是否支持 |
| :-------: | :------: |
|  air001   |    ✅     |
| air32f103 |    🔨     |

## -p, --port <串口名称>

该参数用来指示是所有的串口名称，如`COM12`、`/dev/ttyUSB2`。

## -b, --baud <串口波特率>

该参数用来表示通信时需要使用的波特率，如若使用不带外置晶振的USB转串口芯片导致通信失败，可能需要降低波特率再试，比如`9600`。

# 高级参数

## -t, --trace

如果加上了这个参数，表示会打印出详细的 DEBUG 信息。

## --connect-attempts <次数>

最大重试次数，默认为`10`次，`0`表示无限次。

## --before <default_reset|direct_connect>

下载前的操作，用于让芯片自动进入 BOOT 模式：

- default_reset：Air001 开发板的默认控制方式，由于需要兼容 Arduino IDE 的串口查看器，所以使用了特殊的 ISP 控制电路，进入 BOOT 时需要使用特殊的时序控制方法。
- direct_connect：串口的 DTR 连接 BOOT0 ，RTS 连接 RST，直接控制设备进入 BOOT 模式。

## --after <default_reset|direct_connect>

下载完成后的操作，用于让芯片自动重启运行代码：

- default_reset：Air001 开发板的默认控制方式，原因参考上一小节。
- direct_connect：串口的 RTS 连接 RST，直接控制设备重启。

## 环境变量

### ENG_LANG

当ENG_LANG环境变量为`1`时，将强制将输出打印的信息更改为英文；否则将按系统语言自动判断为中文还是英文。

# 工具命令

## chip_id

获取芯片 ID

## get

获取芯片 ISP 版本和支持的命令列表。

## get_version

获取芯片 ISP 版本和芯片读保护状态。

## write_flash <芯片FLASH地址> <固件文件>

向 FLASH 的指定地址开始，刷入固件，固件文件可以为`HEX`或`BIN`文件。

### write_flash 命令参数

`write_flash`命令有如下参数：

1. `--erase-all`或者`-e`，作用是在烧录的时候擦除全部flash，建议添加。
2. `--no-progress`或者`-p`，作用是在下载的时候禁止显示进度条。

提示

若MCU中已刷入过其他固件，烧录新固件时请务必加上`-e`擦除参数。

## read_unprotect

关闭 FLASH 的读保护，此时将自动擦除 FLASH 上的所有内容。

## read_protect

开启 FLASH 的读保护，此时将无法读取或写入 FLASH 的内容。

## read_flash <芯片FLASH地址> <读取长度> <固件文件>

向 FLASH 的指定地址开始，读取固件存入对应文件，固件文件只能为`BIN`文件。

### read_flash 命令参数

`read_flash`命令有如下参数：

1. `--overwrite`或者`-o`，作用是如果文件已存在，则覆盖文件，否则拒绝继续读取。
2. `--no-progress`或者`-p`，作用是在下载的时候禁止显示进度条。