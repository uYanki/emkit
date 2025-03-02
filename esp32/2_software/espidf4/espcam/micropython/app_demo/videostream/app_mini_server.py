import socket
import cv2
import io
from PIL import Image
import numpy as np
import time

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
s.bind(("0.0.0.0", 9090))

bRecord = False  # True: 启用视频录制功能
if bRecord:
    video_type = cv2.VideoWriter_fourcc(*'XVID')  # 保存为 avi 格式
    video_file = cv2.VideoWriter('%s.avi' % str(time.time()), video_type, 5, (480, 320))

while True:
    data, IP = s.recvfrom(100000)
    bytes_stream = io.BytesIO(data)
    image = Image.open(bytes_stream)
    img = np.asarray(image)
    # 采集的是RGB格式, 要转换为BGR（opencv的格式）
    img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)
    cv2.imshow("ESP32 Capture Image", img)
    if bRecord:
        video_file.write(img)
    if cv2.waitKey(1) == ord("q"):  # 按Q退出
        break
