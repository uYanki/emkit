# WaveGen

from math import sin

with open("wave.txt", "w") as f:
    ctx = ""
    for i in range(0, 360, 15):
        res = int(sin(i / (180 / 3.1415926)) * 2047)
        res += 2047
        ctx += str(res)
        ctx += ","
    f.write(ctx)
