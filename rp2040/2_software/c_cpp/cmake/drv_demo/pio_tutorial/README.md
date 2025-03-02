# [Github](https://github.com/GitJer/Some_RPI-Pico_stuff) 

#### State machine emulator

状态机仿真器：状态机的问题在于，在为sm编写代码时，调试器不能提供我所需要的洞察力。我通常会写一些代码，上传到pico，然后发现它不能完成我想要它做的事情。我不想猜测我做错了什么，我想看看sm执行时寄存器的值。所以，我做了一个[仿真器](https://github.com/GitJer/Some_RPI-Pico_stuff/tree/main/state_machine_emulator).

The problem with the state machines is that debuggers do not give the insight I need when writing code for a sm. I typically write some code, upload it to the pico and find that it doesn't do what I want it to do. Instead of guessing what I do wrong, I would like to see the values of e.g. the registers when the sm is executing. So, I made an [emulator](https://github.com/GitJer/Some_RPI-Pico_stuff/tree/main/state_machine_emulator).

#### Two independently running state machines

两个独立运行的状态机。

[This](https://github.com/GitJer/Some_RPI-Pico_stuff/tree/main/Two_sm_simple) is just an example of two state machines running independently. Nothing special about it, but I had to do it.

#### Two independently running state machines, one gets disabled temporarily

两个独立运行的状态机的例子，但是其中一个被暂时禁用。

[This](https://github.com/GitJer/Some_RPI-Pico_stuff/tree/main/Two_sm_one_disabled) is an example of two state machines running independently, but one gets disabled temporarily.

#### Two independently running state machines, synchronized via irq, one gets disabled temporarily

两个状态机通过设置和清除 irq 来同步的例子，其中一个被暂时禁用。

[This](https://github.com/GitJer/Some_RPI-Pico_stuff/tree/main/Two_sm_one_disabled_with_irq) is an example of two state machines synchronized via setting and clearing an irq, one gets disabled temporarily.

#### State machine writes into a buffer via DMA

状态机通过DMA写入缓冲区。

[This](https://github.com/GitJer/Some_RPI-Pico_stuff/tree/main/sm_to_dma_to_buffer) is an example of a state machine using DMA (Direct Memory Access) to write into a buffer.

#### State Machine -> DMA -> State Machine -> DMA -> Buffer

一个状态机通过DMA向另一个状态机写入数据，后者的输出通过另一个DMA通道放入缓冲区。

[This](https://github.com/GitJer/Some_RPI-Pico_stuff/tree/main/sm_to_dma_to_sm_to_dma_to_buffer) is an example where one state machine writes via DMA to another state machine whose output is put into a buffer via another DMA channel.

#### Communicating values between state machines

在状态机之间传递值。

The [RP2040 Datasheet](https://datasheets.raspberrypi.org/rp2040/rp2040-datasheet.pdf) states that "State machines can not communicate data". Or can they ... Yes they can, in several ways, [including via GPIO pins](https://github.com/GitJer/Some_RPI-Pico_stuff/tree/main/Value_communication_between_two_sm_via_pins).

#### multiply two numbers

乘法。

[This code](https://github.com/GitJer/Some_RPI-Pico_stuff/tree/main/multiplication) multiplies two numbers.

#### Two pio programs in one file

一个文件中使用两个 PIO 程序。

I wanted to see how I could use two pio programs in one file and use them from within the c/c++ program, see [here.](https://github.com/GitJer/Some_RPI-Pico_stuff/tree/main/two_pio_programs_one_file)

#### Subroutines in pioasm

[This code](https://github.com/GitJer/Some_RPI-Pico_stuff/tree/main/subroutines) shows that subroutines in pioasm can be a thing and can - in some cases - be used to do more with the limited memory space than is possible with just writing the code in one program.

---

#### build

```shell
cd build
cmake  ..  -G "MinGW Makefiles"
mingw32-make
```

