# 播放鸡哥的视频

import sensor
import image
import lcd

from pyb import millis

snapshot_source = False  # Set to true once finished to pull data from sensor.
lcd.init(type=2)  # 0.96 lcd
img = sensor.alloc_extra_fb(160, 120, sensor.RGB565)  # 画布
stream = image.ImageIO("/KUN_30fps.bin", "r")

timer = millis()

while (True):
    while millis()-timer < 33:
        pass
    timer = millis()
    img = stream.read(copy_to_fb=True, loop=True, pause=True)
    lcd.display(img)
