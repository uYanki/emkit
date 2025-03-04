`IO`/`Pin`是芯片引出的硬件引脚，`GPIO`是控制这些引脚的外设。

FPIOA: 现场可编程 IO 阵, Field Programmable Input and Output Array。即可将任意的`IO`/`Pin`绑定到`GPIO`上。

GPIO(ID,MODE,PULL,VALUE)

- MODE: GPIO.IN 输入模式，GPIO.OUT 输出模式
- PULL: GPIO.PULL_UP 上拉，GPIO.PULL_DOWN 下拉，GPIO.PULL_NONE 悬空
- VALUE:P 初始电平，1 高电平，0 低电平

GPIO.irq(CALLBACK_FUNC,TRIGGER_CONDITION): 配置中断

- CALLBACK_FUNC: 回调函数
- TRIGGER_CONDITION: 触发方式，GPIO.IRQ_RISING 上升沿，GPIO.IRQ_FALLING 下降沿，GPIO.IRQ_BOTH：上升沿和下降沿

GPIO.disirq(): 关闭中断
