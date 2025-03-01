

# USB 主机

主机使用了操作系统抽象层（osal），当前工程以 FreeRTOS 为例。

## 初始化

1. 选择内部 phy 及启用中断

![image-20250212013933153](.assets/README/image-20250212013933153.png)

![image-20250212013955894](.assets/README/image-20250212013955894.png)

让生成的代码不调用USB初始化函数

![image-20250212014126771](.assets/README/image-20250212014126771.png)

2. 硬件初始化

![image-20250212013801894](.assets/README/image-20250212013801894.png)

注意，此处是使用主机 `usb_hc`，不是从机 `usb_dc`

## 中断相关

1. 不生成 SVC 和 PendSV 中断

![image-20250212013551632](.assets/README/image-20250212013551632.png)

2. 操作系统时钟

![image-20250212013323689](.assets/README/image-20250212013323689.png)

3. USB 时钟

![image-20250212013710645](.assets/README/image-20250212013710645.png)

## 导入源文件

1. 导入 USB 相关文件

![image-20250212014228270](.assets/README/image-20250212014228270.png)

2. 导入操作系统相关文件

![image-20250212014431109](.assets/README/image-20250212014431109.png)

## 测试现象

### KeyBoard

![image-20250212020224334](.assets/README/image-20250212020224334.png)

### Mouse

![image-20250212020248164](.assets/README/image-20250212020248164.png)

### CH340

准备2个CH340，一个接到开发板上，一个接到电脑上。

启用测试代码：

![image-20250212020021026](.assets/README/image-20250212020021026.png)

互相收发：

![image-20250212015951685](.assets/README/image-20250212015951685.png)