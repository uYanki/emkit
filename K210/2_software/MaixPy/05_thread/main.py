import _thread  # 线程
from time import sleep


def thread_cbk(arg):
    while True:
        print("thread {} ".format(arg))
        sleep(1)


# 开启线程，参数须为元组
_thread.start_new_thread(thread_cbk, ("1",))
_thread.start_new_thread(thread_cbk, ("2",))

while True:
    pass
