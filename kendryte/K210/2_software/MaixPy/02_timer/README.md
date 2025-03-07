machine.Timer(id,channel,mode=Timer.MODE_ONE_SHOT,period=1000,unit=Timer.UNIT_MS, callback=None, arg=None, start=True,priority=1, div=0)

- 编号 id: Timer.TIMER0 ~ TIMER2
- 通道 channel: Timer.CHANNEL0 ~ CHANNEL3
- 模式 mode: MODE_ONE_SHOT 一次性, MODE_PERIODIC 周期性, MODE_PWM
- 周期单位 unit: Timer.UNIT_S 秒, Timer.UNIT_MS 毫秒, Timer.UNIT_US 微妙, Timer.UNIT_NS 纳秒
- 硬件中断优先级 priority: 范围是[1,7], 值越小优先级越高
