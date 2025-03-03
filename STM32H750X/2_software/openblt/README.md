[TOC]

# OpenBLT

## 移植记录

### 通讯配置



![image-20250215215947494](.assets/README/image-20250215215947494.png)

#### RS232

* 参数配置

![image-20250215215100309](.assets/README/image-20250215215100309.png)

* 选择 LL 库

![image-20250215215142708](.assets/README/image-20250215215142708.png)

* 在初始化后，调用 OpenBLT 相关函数

![image-20250215215317517](.assets/README/image-20250215215317517.png)

#### USBD

* 默认配置，启用中断

![image-20250215215508021](.assets/README/image-20250215215508021.png)

* 调用 OpenBLT 相关函数

![image-20250215215647954](.assets/README/image-20250215215647954.png)

#### CAN

* TODO

### 接口配置

![image-20250215223415652](.assets/README/image-20250215223415652.png)

### Flash 配置

配置 APP 地址，扇区容量

![image-20250215215844112](.assets/README/image-20250215215844112.png)

