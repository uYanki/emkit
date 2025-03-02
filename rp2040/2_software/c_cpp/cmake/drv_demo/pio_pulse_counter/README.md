# Counting pulses in a pulse train separated by a pause 

This class can be used for protocols where the data is encoded by a number of pulses in a pulse train followed by a pause.
E.g. the LMT01 temperature sensor uses this, [see](https://www.reddit.com/r/raspberrypipico/comments/nis1ew/made_a_pulse_counter_for_the_lmt01_temperature/).

---

test：board-stm32f103rc-berial\7.Example\hal\gpio\pulse_out

使用 stm32 的 IO 来输出脉冲，通过串口2 给定脉冲输出个数。（PB3）



