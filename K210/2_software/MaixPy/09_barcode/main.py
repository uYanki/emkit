# 功能：条形码识别

import sensor
import lcd

lcd.init()
lcd.rotation(2)

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(30)

while True:
    img = sensor.snapshot()
    for i in img.find_barcodes():
        img.draw_string(2, 2, i.payload(), color=(255, 0, 0))
        img.draw_rectangle(i.rect(), color=(255, 0, 0))
    lcd.display(img)
