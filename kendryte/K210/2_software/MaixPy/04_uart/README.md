machine.UART(uart,baudrate,bits,parity,stop,timeout, read_buf_len)
【uart】串口编号。[UART.UART1~UART3]
【baudrate】波特率，常用 115200、9600
【bits】数据位,默认 8
【parity】校验；默认 None, 0(偶校验)，1(奇校验)
【stop】停止位，默认 1
【timeout】串口接收超时时间
【read_buf_len】串口接收缓冲大小。
