from machine import UART
from fpioa_manager import fm

fm.register(6, fm.fpioa.UART1_RX)
fm.register(7, fm.fpioa.UART1_TX)
uart = UART(UART.UART1, 115200, read_buf_len=4096)

uart.write('Hello k210')
while True:
    text = uart.read()
    # uart.read(num)
    # uart.readline(num)
    if text:
        uart.write(text)  # 数据回传
        print(text.decode('utf-8'))  # 解码中文会报错
