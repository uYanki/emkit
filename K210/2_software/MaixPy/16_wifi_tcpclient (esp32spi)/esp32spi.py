import network
from fpioa_manager import fm


class wifi():

    nic = None

    def reset(force=False, reply=5, is_hard=True):
        if force == False and __class__.isconnected():
            return True
        try:
            # IO map for ESP32 on Maixduino
            fm.register(25, fm.fpioa.GPIOHS10)  # cs
            fm.register(8, fm.fpioa.GPIOHS11)  # rst
            fm.register(9, fm.fpioa.GPIOHS12)  # rdy

            if is_hard:
                print("Use Hareware SPI for other maixduino")
                fm.register(28, fm.fpioa.SPI1_D0, force=True)  # mosi
                fm.register(26, fm.fpioa.SPI1_D1, force=True)  # miso
                fm.register(27, fm.fpioa.SPI1_SCLK, force=True)  # sclk
                __class__.nic = network.ESP32_SPI(cs=fm.fpioa.GPIOHS10, rst=fm.fpioa.GPIOHS11, rdy=fm.fpioa.GPIOHS12, spi=1)
                print("ESP32_SPI firmware version:", __class__.nic.version())
            else:
                # Running within 3 seconds of power-up can cause an SD load error
                print("Use Software SPI for other hardware")
                fm.register(28, fm.fpioa.GPIOHS13, force=True)  # mosi
                fm.register(26, fm.fpioa.GPIOHS14, force=True)  # miso
                fm.register(27, fm.fpioa.GPIOHS15, force=True)  # sclk
                __class__.nic = network.ESP32_SPI(cs=fm.fpioa.GPIOHS10, rst=fm.fpioa.GPIOHS11, rdy=fm.fpioa.GPIOHS12, mosi=fm.fpioa.GPIOHS13, miso=fm.fpioa.GPIOHS14, sclk=fm.fpioa.GPIOHS15)
                print("ESP32_SPI firmware version:", __class__.nic.version())

            # time.sleep_ms(500) # wait at ready to connect
        except Exception as e:
            print(e)
            return False
        return True

    def connect(ssid, pwd):
        if __class__.nic != None:
            return __class__.nic.connect(ssid, pwd)

    def ifconfig():  # should check ip != 0.0.0.0
        if __class__.nic != None:
            return __class__.nic.ifconfig()

    def isconnected():
        if __class__.nic != None:
            return __class__.nic.isconnected()
        return False

    def scan():
        enc_str = ["OPEN", "", "WPA PSK", "WPA2 PSK", "WPA/WPA2 PSK", "", "", ""]
        aps = __class__.nic.scan()
        for ap in aps:
            print("SSID:{:^20}, ENC:{:>5} , RSSI:{:^20}".format(ap[0], enc_str[ap[1]], ap[2]))

    def do(ssid, pwd, retry=5):
        if retry < 1:
            retry = 99
        for i in range(retry):
            try:
                __class__.reset(is_hard=True)
                print('try esp32spi connect wifi...')
                __class__.connect(ssid, pwd)
                if __class__.isconnected():
                    break
            except Exception as e:
                print(e)
        if __class__.isconnected():
            print('network state:', wifi.isconnected(), wifi.ifconfig())
            return True
        else:
            return False
