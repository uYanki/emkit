[reboot](https://github.com/jasongaunt/rp2040-bootsel-reboot-example) ：按下 boot 键，使用看门狗复位，进入烧录模式。

## Sample Usage

When this sample application is uploaded to your Pico RP2040 it will print "Hello World" to the USB-CDC Serial device and then start flashing the on-board LED at a rate of 1 Hertz.

Press the BOOTSEL button and release immediately and the Pico RP2040 will reboot and start running the application again.

Press and hold the BOOTSEL button and it will reboot into USB mass storage mode so you can upload a new application.

## Using in your own application

1. In your `CMakeLists.txt` make sure to add `bootsel-reboot.cpp` to the `add_executable()` block.

2. At the top of your application add `#include "bootsel-reboot.hpp"`

3. At the start of your application entry point (usually the `int main()` function) add `arm_watchdog();` after `stdio_init_all();`

4. In your application main loop call `check_bootsel_button();` frequently (ideally every 100 ms or less)

**Warning:** If `check_bootsel_button();` isn't called every 1000 ms or less the watchdog will reboot the Pico RP2040!