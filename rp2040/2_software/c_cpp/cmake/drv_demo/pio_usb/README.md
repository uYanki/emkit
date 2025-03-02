# Pico-PIO-USB

USB host/device implementation using PIO of raspberry pi pico (RP2040).

You can add additional USB port to RP2040.

## Demo

https://user-images.githubusercontent.com/43873124/146642806-bdf34af6-4342-4a95-bfca-229cdc4bdca2.mp4

## Project status

|Planned Features|Status|
|-|-|
|FS Host|✔|
|LS Host|✔|
|Hub support|✔|
|Multi port|✔|
|FS Device|✔|

## Examples

##### Build：


```bash
cd example
mkdir build
cd build
cmake ..
make
# Copy UF2 file in capture_hid_report/ or usbdevice/ to RPiPico
```

---

- [capture_hid_report.c](examples/capture_hid_report/capture_hid_report.c) is a USB host sample program which print HID reports received from device. Open serial port and connect devices to pico. Default D+/D- is gp0/gp1. Call `pio_usb_add_port()` to use additional ports.

RP2040 作主机，捕获 HID 从设备（ 如键盘鼠标）的报文，并通过虚拟串口打印报文（板载的 Mirco USB）。

将 RP2040 的引脚连接至 USB 座子对应引脚上，然后将从设备插入至 USB 座子。

个人使用的是无线鼠标进行测试，OK！

| RP2040 | HID Device |
| ------ | ---------- |
| GP0    | USBDP/D+   |
| GP1    | USBDM/D-   |
| 5V     | VBUS       |
| GND    | GND        |

* [host_hid_to_device_cdc.c](examples/host_hid_to_device_cdc/host_hid_to_device_cdc.c) is similar to **capture_hid_report.c** which print mouse/keyboard report from host port to device port's cdc. TinyUSB is used to manage both device (native usb) and host (pio usb) stack.

程序效果和接线同上方例子，只不过是用了 TinyUSB 协议栈。

* [usb_device.c](examples/usb_device/usb_device.c) is a HID USB FS device sample which moves mouse cursor every 0.5s. External 1.5kohm pull-up register is necessary to D+ pin (Default is gp0).

RP2040 的 PIO USB 连接至 PC， 同时 PIO USB 的 D+ pin 引脚需通过 1.5kΩ 的上拉电阻上拉至 3.3v。

程序运行效果：每隔 500ms 鼠标水平平移一段距离。

（如没效果，将 Pico 重新上电 / 复位即可，推测是程序里没做热插拔的功能）

若 Pico 当前连接的电脑 和 PIO USB 要连接的 PC 是同一台电脑，则 GND 可不接。

| RP2040 | Computer |
| ------ | -------- |
| GP0    | USBDP/D+ |
| GP1    | USBDM/D- |
| GND    | GND      |

* Another sample program for split keyboard with QMK

[https://github.com/sekigon-gonnoc/qmk_firmware/tree/rp2040/keyboards/pico_pico_usb](https://github.com/sekigon-gonnoc/qmk_firmware/tree/rp2040/keyboards/pico_pico_usb)

## Resource Usage

- Two PIO
  - One PIO is for USB transmitter using 22 instruction and one state machine
  - Another PIO is for USB receiver using 31 instruction and two state machine
- Two GPIO for D+/D- (Series 22ohm resitors are better)
- 15KB ROM and RAM
- (For Host) One 1ms repeating timer
- (For Device) One PIO IRQ for receiver
