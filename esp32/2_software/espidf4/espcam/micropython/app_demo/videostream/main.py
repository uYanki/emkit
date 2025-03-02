# 功能：实时显示摄像头画面(帧数也就那样)

import socket
from time import sleep
import camera


def initCamera():
    try:
        if camera.deinit():
            sleep(0.5)
        if camera.init(0, format=camera.JPEG, fb_location=camera.PSRAM):
            sleep(0.5)
            print("success to init camera")
            return True
    except Exception as e:
        pass
    print("fail to init camera")
    return False


def connectWiFi(ssid, pwd):
    import network
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    if not wlan.isconnected():
        wlan.connect(ssid, pwd)
        while not wlan.isconnected():
            print('connecting to network...')
            sleep(0.5)
    print("sucess to connect wifi")
    print(wlan.ifconfig())


if initCamera():

    camera.flip(0)  # 上下翻转
    camera.mirror(1)  # 左右翻转
    camera.framesize(camera.FRAME_HVGA)  # 分辨率 https://bit.ly/2YOzizz
    # FRAME_96X96 FRAME_QQVGA FRAME_QCIF FRAME_HQVGA FRAME_240X240
    # FRAME_QVGA FRAME_CIF FRAME_HVGA FRAME_VGA FRAME_SVGA
    # FRAME_XGA FRAME_HD FRAME_SXGA FRAME_UXGA FRAME_FHD
    # FRAME_P_HD FRAME_P_3MP FRAME_QXGA FRAME_QHD FRAME_WQXGA
    # FRAME_P_FHD FRAME_QSXGA
    camera.speffect(camera.EFFECT_NONE)  # 特效
    # 效果\无（默认）效果\负效果\ BW效果\红色效果\绿色效果\蓝色效果\复古效果
    # EFFECT_NONE (default) EFFECT_NEG \EFFECT_BW\ EFFECT_RED\ EFFECT_GREEN\ EFFECT_BLUE\ EFFECT_RETRO
    camera.whitebalance(camera.WB_HOME)  # 白平衡
    # WB_NONE (default) WB_SUNNY WB_CLOUDY WB_OFFICE WB_HOME
    camera.saturation(0)  # 饱和度[-2,2]（默认0）.2: grayscale,灰度
    camera.brightness(0)  # 亮度[-2,2]（默认0）.2: brightness,最亮
    camera.contrast(0)  # 对比度[-2,2]（默认0）.2: highcontrast,高对比度
    camera.quality(10)  # 质量[10-64] 数字越小质量越高

    # connectWiFi('老八父亲的wifi', 'shengyi318')  # 连接 WiFi
    connectWiFi('uyk', '12345678')  # 连接 WiFi

    udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
    try:
        while True:
            buf = camera.capture()
            # cmd -> ipconfig -> ipv4
            udp.sendto(buf, ("192.168.3.24", 9090))
            sleep(0.1)
    except:
        pass
    finally:
        camera.deinit()
