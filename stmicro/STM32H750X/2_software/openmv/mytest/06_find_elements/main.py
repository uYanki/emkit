import sensor
import time
import lcd

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
if sensor.get_id() == sensor.OV7725:
    sensor.set_hmirror(True)
    sensor.set_vflip(True)
sensor.skip_frames(time=2000)

lcd.init(type=2)  # 0.96 lcd (160x80)

clock = time.clock()
while (True):
    clock.tick()
    img = sensor.snapshot()

    # img.lens_corr(strength=1.8, zoom=1.0)  # 畸变矫正

    # for c in img.find_circles(threshold=4000, x_margin=10, y_margin=10, r_margin=10, r_min=2, r_max=100, r_step=2):
    # for c in img.find_circles():  # 找圆
    #     img.draw_circle(c.x(), c.y(), c.r(), color=(255, 0, 0))  # 量级 c.magnitude(), 值越大说明可信度越高

    # for l in img.find_line_segments(merge_distance=0, max_theta_diff=5):
    # for l in img.find_line_segments():  # 找线段
    #     img.draw_line(l.line(), color=(255, 0, 0))

    # for l in img.find_lines(threshold=1000, theta_margin=25, rho_margin=25):
    # for l in img.find_lines():  # 找直线
    #     if (0 <= l.theta()) and (l.theta() <= 179):  # l.theta() 获取于水平线的夹角
    #         img.draw_line(l.line(), color=(255, 0, 0))

    for r in img.find_rects(threshold=15000):
        img.draw_rectangle(r.rect(), color=(255, 0, 0))
        for p in r.corners():  # 画四角小圆形
            img.draw_circle(p[0], p[1], 5, color=(0, 255, 0))

    lcd.display(img, size_x=160)
    print(clock.fps())  # 显示 FPS（每秒帧数）
