Pico 作为 USB 主机（USB Host），可将 U 盘设备接至板载的 Mirco USB，供 Pico 访问。

通过串口工具连接 Pico，使用 MobaXterm 进行命令行交互。

| Pico | USB-TTL |
| ---- | ------- |
| GP0  | RXD     |
| GP1  | TXD     |
| VBUS | 5V      |
| GND  | GND     |

---

更改，在函数 `msc_demo_cli_init()` 中添加：

```c
demo_config.invitation = "> ";
```

原项目没有该语句，导致时而的内存泄漏，从而无法运行。

