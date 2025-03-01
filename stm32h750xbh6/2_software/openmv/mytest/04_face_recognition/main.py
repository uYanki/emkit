# 功能：人脸识别（效果一般般）

import sensor
import lcd
import time
import image

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
if sensor.get_id() == sensor.OV7725:
    sensor.set_hmirror(True)
    sensor.set_vflip(True)

sensor.set_framesize(sensor.QQVGA)
lcd.init(type=2)  # 0.96 lcd (160x80)

sensor.set_auto_whitebal(True)
sensor.set_auto_gain(True)
sensor.set_contrast(3)
sensor.set_gainceiling(8)

sensor.skip_frames(time=2000)  # 等待稳定

face_cascade = image.HaarCascade("frontalface", stages=25)
eyes_cascade = image.HaarCascade("eye", stages=24)

clock = time.clock()
while (True):
    clock.tick()
    img = sensor.snapshot()

    for face in img.find_features(face_cascade, threshold=0.75, scale_factor=1.25):
        img.draw_rectangle(face, thickness=2)
        for eye in img.find_features(eyes_cascade, threshold=0.75, scale_factor=1.25, roi=face):
            img.draw_rectangle(eye)

    lcd.display(img, x_size=160)
    print(clock.fps())
