# 功能: 色块寻找

import sensor
import lcd
import time

lcd.init()

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_vflip(1)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
sensor.set_auto_exposure(False)

# 颜色识别阈值 (L Min, L Max, A Min, A Max, B Min, B Max) LAB 模型
# 可在`工具->机器视觉->阈值编辑器`进行颜色阈值调试

thresholds = [(30, 100, 15, 127, 15, 127),  # 红
              (30, 100, -64, -8, -32, 32),  # 绿
              (0, 30, 0, 64, -128, -20)]   # 蓝

clock = time.clock()
while True:
    clock.tick()

    img = sensor.snapshot()

    for b in img.find_blobs([thresholds[2]]):  # 0红,1绿,2蓝
        img.draw_rectangle(b[0:4])
        img.draw_cross(b[5], b[6])

    lcd.display(img)

    print(clock.fps())
