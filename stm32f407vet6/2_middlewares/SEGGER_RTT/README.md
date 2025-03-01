# SEGGER Real-Time-Terminal

```
SEGGER_RTT_Conf.h
SEGGER_RTT.h
SEGGER_RTT.c
SEGGER_RTT_printf.c
```

## J-Scope 

HSS：是通过采样周期定时从内存文件中读取变量的值。

* 只能监控全局变量。
* 速度慢，采样速率基本固定在1khz左右。
* 随时可连接MCU，不需添加任何代码。

RTT：类似串口上传数据。

* 更高的数据吞吐量，最高可达2MB/S。