# 功能：控制舵机角度

from machine import Timer, PWM
import time

tim = Timer(Timer.TIMER0, Timer.CHANNEL0, mode=Timer.MODE_PWM)
servo = PWM(tim, freq=50, duty=0, pin=9)


def setServoAngle(servo, angle):
    print(angle)
    servo.duty((angle+90)/180*10+2.5)


while True:
    for i in range(-3, 4):  # -90,90
        setServoAngle(servo, i*30)
        time.sleep(1)
