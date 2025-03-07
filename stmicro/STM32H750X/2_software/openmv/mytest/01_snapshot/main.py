# 功能：照相机（需按RST才能在文件管理器中看到照片）

import sensor
import lcd
import time
from pyb import LED, Pin

USER_LED = LED(1)
USER_KEY = Pin('C13', Pin.IN, Pin.PULL_DOWN)

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
if sensor.get_id() == sensor.OV7725:
    sensor.set_hmirror(True)
    sensor.set_vflip(True)
sensor.set_auto_whitebal(True)  # 白平衡(默认开启)
sensor.set_gainceiling(8)  # 设置增益(官方推荐参数)

sensor.skip_frames(time=2000)
# n = 跳过帧数，time = 等待时间（ms）
# 若 n & time 均没指定时，默认跳过 300ms 的帧。

lcd.init(type=2)  # 0.96 lcd (160x80)
# lcd.init(type=1) # 1.8 lcd (128x160)

count = 0
clock = time.clock()
while (True):
    clock.tick()

    img = sensor.snapshot()
    lcd.display(img)

    if USER_KEY.value() == 1:
        while USER_KEY.value() == 1:  # 等待松开
            USER_LED.on()
            time.sleep(0.05)
            USER_LED.off()
            time.sleep(0.05)
        time.sleep(0.3)
        count += 1
        img.save("image" + str(count)+".jpg")  # 保存照片

    print(clock.fps())
