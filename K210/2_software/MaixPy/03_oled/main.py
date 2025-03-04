from machine import I2C
from ssd1306 import SSD1306

i2c = I2C(I2C.I2C0, mode=I2C.MODE_MASTER, scl=25, sda=24)
oled = SSD1306(i2c, addr=0x3c)

oled.fill(0)  # 清屏, 0x00(白屏)，0xff(黑屏)

# 显示字符 oled.text(str,x,y), x:[0,127]，y:[0,7]（共8行）
oled.text("Hello World!", 0, 0)
oled.text("MicroPython", 0, 2)
oled.text("By 01Studio", 0, 5)
