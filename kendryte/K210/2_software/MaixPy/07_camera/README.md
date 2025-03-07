功能：显示摄像头里的图像（试跑了会，摄像头比芯片还烫）

sensor.reset(): 初始化摄像头

sensor.set_pixformat(pixformat): 设置像素格式

- sensor.GRAYSCAL：灰度图像，每像素 1 字节（8 位）
- sensor.RGB565: 每像素为 2 字节（16 位：5 红，6 绿，5 蓝）

sensor.set_framesize(framesize): 设置图像尺寸

- sensor.QQVGA: 160\*120;
- sensor.QVGA: 320\*240;
- sensor.VGA: 640\*480;

sensor.skip_frames([n, time])

- 摄像头配置后跳过 n 帧或者等待时间 time 让其变稳定。n:跳过帧数；time：等待时间,单位 ms。 （如果 n 和 time 均没指定，则默认跳过 300 毫秒的帧。）

sensor.snapshot()

- 使用相机拍摄一张照片，并返回 image 对象。
