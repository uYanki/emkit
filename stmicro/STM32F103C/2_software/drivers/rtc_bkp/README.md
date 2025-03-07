# 备份寄存器

1. 开启RTC功能

![在这里插入图片描述](.assets/README/5d26394855f93474e6dbe913800ed726.png)

2. API调用

初始化程序已经自动生成好了,直接使用下面的两个函数就可以了

将num数据保存在RTC_BKP_DR1的位置

```c
uint16_t num = 0x25;
HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,num);
```

在RTC_BKP_DR1的位置读取数据

```c
uint16_t num = 0;
num = HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR1);
```

3.  参考资料

![在这里插入图片描述](.assets/README/c3fedbd1c8ab777f22e6a14e96b09441.png)

![在这里插入图片描述](.assets/README/87348c99dae19d16aaae301e2414c9f1.png)

![在这里插入图片描述](.assets/README/b48eac1f9be3f924912413580c81de28.png)

> VBAT引脚需要连接备用电池, 如果VBAT引脚掉电了, BKP的数据也会丢失

