# 功能: 对图像进行卷积运算

import sensor
import lcd
import time

lcd.init(freq=15000000)
lcd.rotation(2)
sensor.reset()
sensor.set_pixformat(sensor.RGB565)  # 不能用 sensor.GRAYSCALE
sensor.set_framesize(sensor.QVGA)
sensor.run(1)

# 卷积核/算子
origin = (0, 0, 0, 0, 1, 0, 0, 0, 0)
edge = (-1, -1, -1, -1, 8, -1, -1, -1, -1)
sharp = (-1, -1, -1, -1, 9, -1, -1, -1, -1)
relievo = (2, 0, 0, 0, -1, 0, 0, 0, -1)
sobel = (-1, 0, 1, -2, 0, 2, -1, 0, 1)

cores = []
cores.append(origin)
cores.append(edge)
cores.append(sharp)
cores.append(relievo)
cores.append(sobel)

for core in cores:
    tim = time.time()
    while True:
        img = sensor.snapshot()
        img.conv3(core)
        lcd.display(img)
        if time.time() - tim > 5:
            break

lcd.clear()
