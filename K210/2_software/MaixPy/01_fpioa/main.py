# for MaixBIT

from Maix import GPIO
from fpioa_manager import fm
from utime import sleep, sleep_ms, sleep_us

'''LED'''
PIN_LED_R = 13
PIN_LED_G = 12
PIN_LED_B = 14

fm.register(PIN_LED_R, fm.fpioa.GPIO0)
fm.register(PIN_LED_G, fm.fpioa.GPIO1)
fm.register(PIN_LED_B, fm.fpioa.GPIO2)

LED_R = GPIO(GPIO.GPIO0, GPIO.OUT, value=0)
LED_G = GPIO(GPIO.GPIO1, GPIO.OUT, value=0)
LED_B = GPIO(GPIO.GPIO2, GPIO.OUT, value=0)

'''KEY'''


def key_cbk(gpio):
    sleep_ms(5)  # 消抖
    if gpio.value() == 0:
        print("key_boot")


PIN_KEY_BOOT = 16

fm.register(PIN_KEY_BOOT, fm.fpioa.GPIOHS0)
KEY_BOOT = GPIO(GPIO.GPIOHS0, GPIO.IN, GPIO.PULL_UP)
KEY_BOOT.irq(key_cbk, GPIO.IRQ_FALLING)

'''LOOP'''

while True:
    LED_R.value(0)
    sleep(1)  # s
    LED_R.value(1)

    LED_G.value(0)
    sleep_ms(1_000)  # ms
    LED_G.value(1)

    LED_B.value(0)
    sleep_us(1_000_000)  # us
    LED_B.value(1)

# fm.unregister(PIN_LED_R)
# fm.unregister(PIN_LED_G)
# fm.unregister(PIN_LED_B)
# fm.unregister(PIN_KEY_BOOT)
