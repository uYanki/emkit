# 功能：视频录制（需插入SD卡）
#
# 提示: 你需要插入SD卡来运行此程序.
#
# 翻译和注释：01Studio

import video
import sensor
import lcd
import time

# 摄像头

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_vflip(1)
sensor.skip_frames(30)

# 屏幕

lcd.init()

# 保存路径

v = video.open("/sd/new.avi", record=1, interval=100000, quality=50)  # 帧间隔：100000 = 100ms

i = 0  # 计算录制帧数
while True:
    tim = time.ticks_ms()
    img = sensor.snapshot()
    lcd.display(img)
    len = v.record(img)  # 返回的录制帧长度。
    print("record", time.ticks_ms() - tim)  # 打印录制的每帧间隔

    i += 1
    if i > 50:  # 50 * 100ms =5s
        break

v.record_finish()  # 停止录制

print("finish")
