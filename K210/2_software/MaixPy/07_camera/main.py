# 功能：照相机(需插入TF卡)

import sensor
import lcd
import time
from Maix import GPIO
from fpioa_manager import fm

##### 按键 #####

key_state = False


def key_cbk(gpio):
    global key_state
    time.sleep_ms(10)  # 消抖
    if gpio.value() == 0:
        key_state = True
        print("key_boot")


PIN_KEY_BOOT = 16
fm.register(PIN_KEY_BOOT, fm.fpioa.GPIOHS0)
KEY_BOOT = GPIO(GPIO.GPIOHS0, GPIO.IN, GPIO.PULL_UP)
KEY_BOOT.irq(key_cbk, GPIO.IRQ_FALLING)  # 下降沿触发中断

##### 摄像头 #####

lcd.init(freq=15000000)

sensor.reset()
sensor.set_vflip(1)
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)  # 320x240
sensor.skip_frames(time=1000)
# sensor.run(0)

count = 1
clock = time.clock()
while (True):
    clock.tick()
    img = sensor.snapshot()
    img.rotation_corr(x_rotation=10, y_rotation=45)  # 旋转校正, 图像上在三维的视角旋转 rotation_correction
    img.draw_string(0, 0, 'FPS: '+str(clock.fps()), color=(255, 255, 255), scale=1, mono_space=False)  # show fps
    lcd.display(img)  # show image
    if key_state:
        img.save("/sd/image"+str(count)+".jpg")
        count += 1  # 编号递增
        print("Done! Reset the camera to see the saved image.")
        time.sleep(2)  # 观察照片
        key_state = False
