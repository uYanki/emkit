from machine import Timer, PWM
from utime import sleep_ms


def timer_cbk(tim):
    print(tim.callback_arg())


# TIMER
tim = Timer(Timer.TIMER0,
            Timer.CHANNEL0,
            mode=Timer.MODE_PERIODIC,  # 定时器模式
            period=1000,  # 时钟周期
            unit=Timer.UNIT_MS,  # 时钟周期单位
            callback=timer_cbk,  # 回调函数
            start=False,  # 是否立即开启定时器
            arg="hello k210"  # 参数
            )
tim.restart()  # 启动定时器
sleep_ms(4500)
tim.deinit()
