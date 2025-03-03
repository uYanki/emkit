ICP（In-Circuit Programming）：烧录器 SWD

ISP（In System Programing）：芯片出厂是带有的引导程序 Bootloader

IAP（In-Application Programming）： 软件自身实现 Bootloader

# ISP

调用系统存储器中 Bootloader，使用 STM32CubeProgramer 通过 USB/UART 更新程序。

![st-img](.assest/README/054352xo3a3m936r99amnn.png)

# IAP

![Screenshot_1](.assest/README/Screenshot_1.png)



![Screenshot 2023-06-25 161810](.assest/README/Screenshot 2023-06-25 161810.png)



## BIOS

1. 用户程序地址

![app_addr](.assest/README/app_addr.png)

2. 串口设定

![set_uart_port](.assest/README/set_uart_port.png)

## APP

1. 设置固件区域

![set_fw_offset](.assest/README/set_fw_offset.png)

2. 设置中断向量表偏移

![enable_user_vert_tab_address](.assest/README/enable_user_vert_tab_address.png)

![set_vect_tab_offset](.assest/README/set_vect_tab_offset.png)

## Xshell

YModem-1K