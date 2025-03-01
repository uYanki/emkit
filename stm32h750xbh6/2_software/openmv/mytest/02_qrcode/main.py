# 二维码识别

import sensor
import time
import lcd

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
if sensor.get_id() == sensor.OV7725:
    sensor.set_hmirror(True)
    sensor.set_vflip(True)
sensor.set_contrast(3)

lcd.init(type=2)  # 0.96 lcd

clock = time.clock()
while True:
    clock.tick()
    img = sensor.snapshot()
    for code in img.find_qrcodes():
        print(code)
        img.draw_rectangle(code[0], code[1], code[2], code[3], color=(255, 0, 0), fill=False)
        img.draw_string(10, 10, code[4], scale=3, x_spacing=1, mono_space=False)
        print(clock.fps())
    lcd.display(img, x_size=160)
