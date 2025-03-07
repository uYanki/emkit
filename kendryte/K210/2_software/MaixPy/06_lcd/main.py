# 功能：屏幕绘制测试

import lcd
import image


lcd.init(freq=15000000, color=lcd.WHITE)
# lcd.init(type=1, width=320, height=240, invert=True, freq=20000000) # invert 反色

# lcd.clear(lcd.BLACK)
# lcd.mirror(True)  # 镜像
# lcd.rotation(2)  # 旋转, 取值[0-3]

# 能显示位深度为24比特的BMP格式图像和能显示JPG格式图像（不能显示PNG格式图像）
img = image.Image("maixbit.bmp", copy_to_fb=True)
img.draw_line(20, 20, 100, 20, color=(255, 0, 0), thickness=2)
img.draw_rectangle(150, 20, 100, 30, color=(0, 255, 0),  thickness=2, fill=False)
img.draw_circle(60, 120, 30, color=(0, 0, 255), thickness=2, fill=False)
img.draw_arrow(150, 120, 250, 120, color=(255, 0, 255), size=20, thickness=2)
img.draw_cross(60, 200, color=(255, 255, 0), size=20,  thickness=2)
img.draw_string(150, 200, "Hello Maix-Bit!", color=(128, 128, 128), scale=2, mono_space=False)

# img = image.Image(size=(240,240))
# img.draw_rectangle(0,0,30, 240, fill=True, color=(0xff, 0xff, 0xff))
# img.draw_rectangle(30,0,30, 240, fill=True, color=(250, 232, 25))
# img.draw_rectangle(60,0,30, 240, fill=True, color=(106, 198, 218))
# img.draw_rectangle(90,0,30, 240, fill=True, color=(98, 177, 31))
# img.draw_rectangle(120,0,30, 240, fill=True, color=(180, 82, 155))
# img.draw_rectangle(150,0,30, 240, fill=True, color=(231, 47, 29))
# img.draw_rectangle(180,0,30, 240, fill=True, color=(32, 77, 158))
# img.draw_rectangle(210,0,30, 240, fill=True, color=(27, 28, 32))

lcd.display(img, (0, 0, 320, 240))  # 裁剪显示
