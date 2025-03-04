# 功能：口罩检测（需烧录带`kmodel_v4`的固件）

import sensor
import image
import lcd
import time
import KPU as kpu

color_R = (255, 0, 0)
color_G = (0, 255, 0)
color_B = (0, 0, 255)

class_IDs = ['no_mask', 'mask']

lcd.init()
lcd.rotation(2)

sensor.reset(dual_buff=True)
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)

# task = kpu.load(0x300000)
task = kpu.load('/sd/maskdetect.smodel')

anchor = (0.1606, 0.3562, 0.4712, 0.9568, 0.9877, 1.9108, 1.8761, 3.5310, 3.4423, 5.6823)
kpu.init_yolo2(task, 0.5, 0.3, 5, anchor)
img_lcd = image.Image()

clock = time.clock()
while (True):
    clock.tick()

    img = sensor.snapshot()
    code = kpu.run_yolo2(task, img)

    if code:

        for item in code:
            confidence = float(item.value())
            itemROL = item.rect()
            classID = int(item.classid())

            if confidence < 0.52:
                # img.draw_rectangle(itemROL, color=color_B, tickness=5)
                continue

            bMask = classID == 1 and confidence > 0.65
            color = color_G if bMask else color_R  # 带口罩画绿框, 不带口罩画红框
            text = ('mask: ' if bMask else 'no_mask: ') + str(int(confidence*100)) + '%'
            img.draw_rectangle(itemROL, color, tickness=5)
            img.draw_string(item.x(), item.y(), text, color=color)

    lcd.display(img)

    print(clock.fps())

kpu.deinit(task)
