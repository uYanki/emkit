# 功能：二维码识别

import sensor
import time
import lcd

lcd.init()
lcd.rotation(2)

sensor.reset()
# sensor.set_vflip(1) # 开启会导致识别不了二维码
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.QVGA)

clock = time.clock()
while True:
    clock.tick()

    img = sensor.snapshot()
    for code in img.find_qrcodes():
        img.draw_rectangle(code.rect())
        img.draw_string(code.x(), code.y(), code.payload(), color=lcd.WHITE, scale=2)

    print(clock.fps())
    lcd.display(img)
