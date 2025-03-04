# 功能：采集音频并显示其频谱

import image
import lcd
from fpioa_manager import fm
from Maix import I2S, FFT

# FFT 参数

lcd_height = 240
sample_rate = 38640  # 采样率
sample_points = 1024  # 采样点数
fft_points = 512  # FFT运算点数

# 条形柱

hist_x_num = 50  # 数量
if hist_x_num > 320:
    hist_x_num = 320
hist_width = int(320 / hist_x_num)  # 宽度

# 屏幕

lcd.init()

# 缓冲区

img = image.Image()

# 麦克风

fm.register(20, fm.fpioa.I2S0_IN_D0, force=True)
fm.register(19, fm.fpioa.I2S0_WS, force=True)
fm.register(18, fm.fpioa.I2S0_SCLK, force=True)
rx = I2S(I2S.DEVICE_0)
rx.channel_config(rx.CHANNEL_0, rx.RECEIVER, align_mode=I2S.STANDARD_MODE)
rx.set_sample_rate(sample_rate)

# 主循环

while True:

    audio = rx.record(sample_points)  # 采集音频
    fft_res = FFT.run(audio.to_bytes(), fft_points)  # FFT
    fft_amp = FFT.amplitude(fft_res)  # 频谱幅值
    img = img.clear()

    x_shift = 0  # 横坐标（频率）
    for i in range(hist_x_num):
        hist_height = lcd_height if fft_amp[i] > lcd_height else fft_amp[i]  # 高度限制
        img = img.draw_rectangle((x_shift, 240-hist_height, hist_width, hist_height), [255, 255, 255], 2, True)  # 绘制实心矩形
        x_shift = x_shift + hist_width

    lcd.display(img)
    fft_amp.clear()
