import pywinusb.hid as hid
import time

# VID and PID customization changes here...

VID = 0x34B7
PID = 0xFFFF

# Send buffer
buffer = [0x00]*64

# Const
TIMEOUT = -1
PASS = 0
FAIL = 1

# Result
result = TIMEOUT

I2C_FUNC_TX = 0
I2C_FUNC_RX = 1
I2C_FUNC_WR = 2
I2C_FUNC_RD = 3
I2C_FUNC_SCAN = 4

I2C_FLAG_ADDR_10BIT = 1 << 2
I2C_FLAG_NO_START = 1 << 4
I2C_FLAG_NO_STOP = 1 << 7

I2C_FLAG_DEV_ADDR_10BIT = 1 << 2
I2C_FLAG_MEM_ADDR_16BIT = 1 << 15
I2C_FLAG_NO_START = 1 << 4
I2C_FLAG_NO_STOP = 1 << 7
I2C_FLAG_NO_RD_ACK = 1 << 6
I2C_FLAG_CHK_WR_ACK = 1 << 8


def search_dev():
    filter = hid.HidDeviceFilter(vendor_id=VID, product_id=PID)
    hid_device = filter.get_devices()
    return hid_device


def recv_data(data):

    func = data[1]

    if func == I2C_FUNC_SCAN:

        count = data[2] | (data[3] << 8)

        if count == 0:
            print("no i2c device\n")
        else:
            print(f"find {count} i2c device\n")
            for i in range(127):
                if data[4+int(i/8)] & (1 << (i % 8)):
                    print("0x{:02X}".format(i))

        pass
    elif func == I2C_FUNC_TX:
        pass
    elif func == I2C_FUNC_RX:
        pass
    elif func == I2C_FUNC_WR:
        pass
    elif func == I2C_FUNC_RD:
        pass

    # print(data)
    return None


def send_report(report):
    buffer[0] = report[0].report_id
    report[0].set_raw_data(buffer)
    report[0].send()


def i2c_tx(report, address, xferdata, flags):

    xfersize = 0

    buffer[1] = I2C_FUNC_TX
    buffer[2] = (address & 0x00FF) >> 0
    buffer[3] = (address & 0xFF00) >> 8

    if xferdata != None:
        xfersize = len(xferdata)

    buffer[4] = (xfersize & 0x00FF) >> 0
    buffer[5] = (xfersize & 0xFF00) >> 8

    for i in range(xfersize):
        buffer[6+i] = xferdata[i]

    buffer[6+xfersize] = (flags & 0x00FF) >> 0
    buffer[7+xfersize] = (flags & 0xFF00) >> 8

    send_report(report)


def i2c_rx(report, address, xfersize, flags):

    buffer[1] = I2C_FUNC_RX
    buffer[2] = (address & 0x00FF) >> 0
    buffer[3] = (address & 0xFF00) >> 8
    buffer[4] = (xfersize & 0x00FF) >> 0
    buffer[5] = (xfersize & 0xFF00) >> 8
    buffer[6] = (flags & 0x00FF) >> 0
    buffer[7] = (flags & 0xFF00) >> 8

    send_report(report)


def i2c_write_mem(report, devaddr, memaddr, xferdata, flags):

    xfersize = 0

    buffer[1] = I2C_FUNC_WR
    buffer[2] = (devaddr & 0x00FF) >> 0
    buffer[3] = (devaddr & 0xFF00) >> 8
    buffer[4] = (memaddr & 0x00FF) >> 0
    buffer[5] = (memaddr & 0xFF00) >> 8

    if xferdata != None:
        xfersize = len(xferdata)

    buffer[6] = (xfersize & 0x00FF) >> 0
    buffer[7] = (xfersize & 0xFF00) >> 8

    for i in range(xfersize):
        buffer[8+i] = xferdata[i]

    buffer[8+xfersize] = (flags & 0x00FF) >> 0
    buffer[9+xfersize] = (flags & 0xFF00) >> 8

    send_report(report)


def i2c_read_mem(report, devaddr, memaddr, xfersize, flags):
    buffer[1] = I2C_FUNC_RD
    buffer[2] = (devaddr & 0x00FF) >> 0
    buffer[3] = (devaddr & 0xFF00) >> 8
    buffer[4] = (memaddr & 0x00FF) >> 0
    buffer[5] = (memaddr & 0xFF00) >> 8
    buffer[6] = (xfersize & 0x00FF) >> 0
    buffer[7] = (xfersize & 0xFF00) >> 8
    buffer[8] = (flags & 0x00FF) >> 0
    buffer[9] = (flags & 0xFF00) >> 8

    send_report(report)


def i2c_scan(report):
    buffer[1] = I2C_FUNC_SCAN
    send_report(report)


def oled_test(report, devaddr, width, height):

    WR_CMD = 0x00
    WR_DATA = 0x40

    COM_PIN_CFG = 0x02 if height == 32 else 0x12
    SET_CONTRAST = 0x81
    SET_ENTIRE_ON = 0xa4
    SET_NORM_INV = 0xa6
    SET_DISP_OFF = 0xae
    SET_DISP_ON = 0xaf
    SET_MEM_ADDR = 0x20
    SET_COL_ADDR = 0x21
    SET_PAGE_ADDR = 0x22
    SET_DISP_START_LINE = 0x40
    SET_SEG_REMAP_ON = 0xa1
    SET_MUX_RATIO = 0xa8
    SET_COM_OUT_DIR_08 = 0xc8
    SET_DISP_OFFSET = 0xd3
    SET_COM_PIN_CFG = 0xda
    SET_DISP_CLK_DIV = 0xd5
    SET_PRECHARGE = 0xd9
    SET_VCOM_DESEL = 0xdb
    SET_CHARGE_PUMP = 0x8d

    initcmds = [
        [SET_DISP_OFF],
        [SET_MEM_ADDR, 0x00],
        [SET_DISP_START_LINE],
        [SET_SEG_REMAP_ON],
        [SET_MUX_RATIO, height - 1],
        [SET_COM_OUT_DIR_08],
        [SET_DISP_OFFSET, 0x00],
        [SET_COM_PIN_CFG, COM_PIN_CFG],
        [SET_DISP_CLK_DIV, 0x80],
        [SET_PRECHARGE, 0xf1],
        [SET_VCOM_DESEL, 0x30],
        [SET_CONTRAST, 0xff],
        [SET_ENTIRE_ON],
        [SET_NORM_INV],
        [SET_CHARGE_PUMP, 0x14],
        [SET_DISP_ON],
        [SET_COL_ADDR, 0, width-1],
        [SET_PAGE_ADDR, 0, int(height/8)-1],
    ]

    for cmd in initcmds:
        i2c_write_mem(report, devaddr, WR_CMD,  cmd, 0)

    tab = [0x0F] * 32
    step = width//len(tab)

    for i in range(height//8):
        for j in range(0, width, step):
            i2c_write_mem(report, devaddr, WR_DATA,  tab, 0)


if __name__ == '__main__':

    devaddr = 0x50

    device = search_dev()[0]
    device.open()
    device.set_raw_data_handler(recv_data)
    report = device.find_output_reports()

    i2c_scan(report)
    time.sleep(2)

    oled_test(report, 0x3C, 128, 64)

    # i2c_read_mem(report, devaddr, 0, 30, I2C_FLAG_MEM_ADDR_16BIT)
    time.sleep(2)
    device.close()

    # txdat = [1, 2, 3, 4, 5, 1, 2, 3, 4, 5]
    # i2c_write_mem(report, devaddr, 0, txdat, I2C_FLAG_MEM_ADDR_16BIT)
    # time.sleep(0.5)
