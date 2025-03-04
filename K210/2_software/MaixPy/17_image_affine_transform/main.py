# 功能：对图像进行仿射变换

import image
import lcd
import sensor
import time

lcd.init()

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)

matrix = image.get_affine_transform([(0, 0), (240, 0), (240, 240)], [(60, 60), (240, 0), (220, 200)])
print("matrix:")  # 输出矩阵
print("[{:.02f}, {:.02f}, {:.02f}]".format(matrix[0], matrix[1], matrix[2]))
print("[{:.02f}, {:.02f}, {:.02f}]".format(matrix[3], matrix[4], matrix[5]))
print("[{:.02f}, {:.02f}, {:.02f}]".format(matrix[6], matrix[7], matrix[8]))

try:
    del img_dst
    del img_src
except Exception:
    pass

flag = False
img_dst = image.Image(size=(320, 240))
img_dst.pix_to_ai()

while True:
    img_src = sensor.snapshot()
    image.warp_affine_ai(img_src, img_dst, matrix)
    img_dst.ai_to_pix()
    lcd.display(img_dst if flag else img_src)
    flag = not flag
    time.sleep_ms(500)
