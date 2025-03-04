# 功能：TCP 客户端

import socket
from esp32spi import wifi


def run_tcpclient(ip, port):
    sock = socket.socket()
    sock.connect((ip, port))
    sock.settimeout(1)  # 超时时间,单位秒
    while 1:
        sock.send("hello\r\n")
        try:
            data = b""
            while True:
                tmp = sock.recv(1)
                if len(tmp) == 0:
                    raise Exception('timeout or disconnected')
                data += tmp
        except Exception as e:
            print("rcv:", len(data), data)
    sock.close()


if __name__ == "__main__":
    wifi.reset(is_hard=True)
    wifi.scan() # 扫描 wifi
    if wifi.do("老八父亲的wifi", "shengyi318"):
        run_tcpclient("192.168.3.24", 6000)
