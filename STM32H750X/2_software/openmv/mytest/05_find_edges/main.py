import sensor
import image
import time
import lcd

sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.QQVGA)
if sensor.get_id() == sensor.OV7725:
    sensor.set_hmirror(True)
    sensor.set_vflip(True)
sensor.set_gainceiling(8)
sensor.skip_frames(time=2000)

lcd.init(type=2)  # 0.96 lcd (160x80)

clock = time.clock()
while (True):
    clock.tick()
    img = sensor.snapshot()
    img.find_edges(image.EDGE_CANNY, threshold=(50, 80))  # Canny 算子
    lcd.display(img, size_x=160)
    print(clock.fps())  # 显示 FPS（每秒帧数）
