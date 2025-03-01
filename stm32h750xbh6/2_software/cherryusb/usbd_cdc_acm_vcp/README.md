[TOC]

# 虚拟串口 

## CubeMX

### 时钟配置

![image-20250208001524709](.assets/README/image-20250208001524709.png)

### USB配置

设备模式 + USB中断

![image-20250208001413612](.assets/README/image-20250208001413612.png)

![image-20250208001509501](.assets/README/image-20250208001509501.png)

## 移植过程

### 协议栈导入

添加配置文件

![image-20250208003837420](.assets/README/image-20250208003837420.png)

添加以下头文件路径至项目

![image-20250208003652710](.assets/README/image-20250208003652710.png)

添加以下源文件至项目

![image-20250208002713964](.assets/README/image-20250208002713964.png)



* 根据芯片系列的选择底层接口

`middlewares\cherryusb\port\fsdev\README.md`

![image-20250208003238118](.assets/README/image-20250208003238118.png)

 `middlewares\cherryusb\port\dwc2\README.md` 

![image-20250208003110271](.assets/README/image-20250208003110271.png)

### 接口配置

* 初始化 `main.c` 

```c
void usb_dc_low_level_init(void)
{
	MX_USB_OTG_FS_PCD_Init();
}

void usb_dc_low_level_deinit(void){}
```

* 中断函数 `stm32h7xx_it.c` 

![image-20250208001738477](.assets/README/image-20250208001738477.png)

```c
extern void USBD_IRQHandler(uint8_t busid);
USBD_IRQHandler(0);
```

## 测试例程

1. 导入模板

![image-20250208004244773](.assets/README/image-20250208004244773.png)

2. 添加测试代码

![image-20250208004342262](.assets/README/image-20250208004342262.png)

```c
cdc_acm_init(0, USB_OTG_FS_PERIPH_BASE);
cdc_acm_data_send_with_dtr_test(0);
HAL_Delay(500);
```

3. 测试效果

![image-20250208004213172](.assets/README/image-20250208004213172.png)

4. 附加说明

例程默认只有勾选 DTR 时，MCU 才会发送数据。

若想不勾选  DTR 时，MCU 也发送数据，需屏蔽 DTR 状态检测：

![image-20250208004738044](.assets/README/image-20250208004738044.png)