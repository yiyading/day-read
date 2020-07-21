# 一、计数器介绍

源地址：https://blog.csdn.net/zoomdy/article/details/79443472

RISC-V定义了3个64位计数器，分别为：cycle、time、instret，这三个寄存器可以用来评估硬件性能，还可以产生嘀嗒定时中断。
> 本实验中只测试了时间计数功能。

## 1. 64位计数器
cycle、time、instret这三个计数器都是64位的，无论是RV32I、RV64I还是RV128I。在RV32I/RV32E中，访问64位计数器要注意正好碰上低32位进位的情况，参考[参考《RV32I/RV32E在访问mtime和mtimecmp的注意事项》](https://blog.csdn.net/zoomdy/article/details/79361553)
> 这三个寄存器的主要作用是评估硬件性能的，同时time计数器配合timecmp可以提供一个嘀嗒定时器。

## 2. 计数器
**统计的都是机器周期？**

时钟周期 -> 机器周期 -> 指令周期
> 1. 几个时钟周期组成一个机器周期，一个时钟周期完成一个基本**动作**。<br>
> 2. 几个机器周期组成一个指令周期，一个机器周期完成一个基本**操作**。<br>
> 3. 一个指令周期表示取出一条指令并完成该条指令的时间。<br>
> > 指令周期是从取指令、分析指令到执行完成所需要的的全部时间。<br>

1. cycle计数器：统计自CPU复位以来共运行了多少个周期。
2. time计数器：统计自CPU复位以来共运行了多少时间，驱动time计数器是已知的固定频率的时钟，例如32768Hz的时钟。
3. instret计数器：统计自CPU复位以来共运行了多少条指令。

## 3. cycle vs time
cycle统计的是CPU周期数，驱动cycle计数器的是CPU的核心时钟，核心时钟可能是动态调整的，例如繁忙状态下核心时钟调整到100MHz，空闲状态下核心时钟调整到10MHz，依据cycle是无法确定CPU运行了多少时间的，除非CPU的时钟是固定的。而驱动time计数器的一定是固定频率的时钟，所以可以用来确定CPU运行了多少时间。

## 4. cycle vs instret
cycle统计的是周期数，instret统计的是指令数，有些指令需要多个周期才能完成，例如MUL指令，有些实现需要4个周期，那么执行MUL指令后，cycle增加4，而instret增加1。还有就是内存访问会引起等待周期，内存的等待周期会累计到cycle，但是不影响instret。

## 5. 读计数器指令
```c
RDCYCLE    rd // ReaD CYCLE
RDCYCLEH   rd // ReaD CYCLE upper Half, RV32I/RV32E only
RDTIME     rd // ReaD TIME
RDTIMEH    rd // ReaD TIME upper Half, RV32I/RV32E only
RDINSTRET  rd // ReaD INSTR RETired
RDINSTRETH rd // ReaD INSTR upper Half, RV32I/RV32E only
```
## 6. 测试源代码
```c
#include<stdio.h>
#include<stdarg.h>
#include<time.h>
#include<inttypes.h>

int main()
{
        printf("\tcycle\t\ttime\t instret\t counter exm!i\n");

        unsigned long cycle_start, cycle_end;
        unsigned long time_start,  time_end;
        unsigned long instret_start, instret_end;
        int tmp = 0;

        asm volatile("rdcycle %0" : "=r"(cycle_start));
        // unsigned long c1 = cycle_start + time_start;
        // unsigned long c11 = cycle_start + time_start;
        // unsigned long c111 = cycle_start + time_start;
        tmp += 1;
        asm volatile("rdcycle %0" : "=r"(cycle_end));

        asm volatile("rdtime %0" : "=r"(time_start));
        // unsigned long c2 = cycle_start + time_start;
        // unsigned long c22 = cycle_start + time_start;
        // unsigned long c222 = cycle_start + time_start;
        tmp += 1;
        asm volatile("rdtime %0" : "=r"(time_end));

        asm volatile("rdinstret %0" : "=r"(instret_start));
        // unsigned long c3 = cycle_start + time_start;
        // unsigned long c33 = cycle_start + time_start;
        // unsigned long c333 = cycle_start + time_start;
        tmp += 1;
        asm volatile("rdinstret %0" :  "=r"(instret_end));

        printf("end:%lu, %lu, %lu\r\n", cycle_end, time_end, instret_end);
        printf("start: %lu, %lu, %lu\r\n", cycle_start, time_start, instret_start);
        printf("time:  %lu, %lu, %lu\r\n", cycle_end - cycle_start, time_end - time_start, instret_end - instret_start);
        return 0;
}
```
