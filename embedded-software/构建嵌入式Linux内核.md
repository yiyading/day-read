# 嵌入式Linux内核
完整版或发行套件通常不能简单用于嵌入式系统
1. 内核太大，有些内容用不上，系统过于复杂，调试维护困难等系统资源受限，尤其是内存方面
2. 系统架构方面的特殊考虑，如需要arm平台的一些特殊考虑
3. 系统性能方面的特殊考虑，如实时性
4. 如果内核来源不是官方内核，包含的内容是很难确定的
5. 功能选择方面的考虑，如文件系统
6. 总之，应该针对嵌入式系统的实际需求构建内核！

一些特殊用途或专用平台
1. Linuxhttp://www.raspbian.org/
2. http://www.arm.linux.org.uk/
3. http://www.uclinux.org/
4. http://www.linux-mips.org/
5. http://www.linux-m68k.org/

什么是嵌入式Linux
1. 嵌入式Linux内核 - 为特殊的硬件配置、或为了支持特别的应用、或为了满足特定的系统需求而裁剪过的Linux内核，通常具有不同于(有时非常不同于)工作站与服务器的内核配置。
2. 嵌入式Linux发行套件 - 开发嵌入式Linux系统的平台，配有各种为了在嵌入式系统中使用而剪裁过的应用软件
3. 需要一套专门的用于嵌入式软件开发的工具集，包括交叉编译器，交叉调试器，软件管理工具，bootloader，模拟器/仿真器等

Linux内核可以做的调整
1. Reduce kernel size
2. Improve the system startup speed
3. Use device driver modules
4. Use light-weight-GUI
5. Remove Virtual File System
6. Remove Virtual Memory Management

# 基于Linux的嵌入式操作系统
常见的Linux嵌入式操作系统：Red Hat/eCos、MontaVista Linux、LynuxWorks/Blue Cat Linux (RT)、Embedix、μCLinux、ARMLinux、RT-Linux、RED-Linux、Kurt-Linux、Red Flag、Raspbian、其他

## Linux实时性问题
实时性影响因素
1. 虚拟内存管理
2. 可剥夺内核
3. 进程调度
4. 系统时钟精度
5. 中断延迟
6. 自旋锁（spinlock）

改进方法
1. 单内核架构：如Linux内核实时补丁PREEMPT_RT
2. 双内核架构：如RT-Linux，RTAI等

## μCLinux
大多数嵌入式处理器不采用虚拟内存技术
1. 虚拟内存技术是非常消耗时间的
2. 有些嵌入式处理器不配置内存管理单元MMU

最初μCLinux 是基于Linux2.0内核的嵌入式Linux版本，是专为没有MMU的微处理器(例如 Coldfir， ARM7TDMI)而设计的，后来的版本包括了2.4, 2.6以上内核

和Linux的主要区别在于内存管理和进程管理部分，其内核远小于通用Linux内核

μCLinux并非为解决Linux的实时性缺陷而提出的，因此，对实时应用的支持仍显不足，但可以很好地应用于实时性要求不是很强的系统

μCLinux的文件系统
> μCLinux多采用romfs文件系统，具有相对简单、占用内存少的特点<br>
> * omfs是read-only系统，系统需要虚拟盘(RAM Disk)支持中间数据的存储，它是早期专为Linux设计的块结构文件系统<br>
> <br>
> 日志文件系统在μCLinux中得到较广泛的应用，其中以支持NOR FLASH的JFFS、JFFS2文件系统和支持NAND FLASH的YAFFS最为流行<br>
> * JFFS2是一个日志文件结构，它的最大特点是按时间顺序存储更新数据和元数据节<br>
> * 日志结构的文件系统在系统出现问题时比较容易恢复

μCLinux内存管理和进程创建
> μCLinux以平板存储方式支持没有MMU的嵌入式处理器，不能使用虚拟存储技术<br>
> <br>
> 在μCLinux中，所有子进程只能用vfork()创建，其进程实质上类似于线程<br>
> * 注：Linux中创建进程有三种方法，fork(), vfork(), clone()。fork()创建进程，vfork()创建类似于线程的进程，clone()创建轻量级进程

## RT-Linux
RT-Linux是为改进Linux的实时性而提出的。主要思想为：在Linux的基础上单独设计一个可剥夺实时性微内核，由这个微内核来管理实时进程。

Linux本身也运行在该微内核之上，相当于最低优先级的一个实时进程。

实时进程交给微内核管理，非实时进程由普通Linux内核处理。

RT-Linux关键技术
> RT-Linux实时内核的关键技术是一个用软件实现的中断控制器。<br>
> * 当Linux要关闭处理器的中断时，该中断控制器就截取这个请求，但并不真正地去关闭处理器的中断，而是基于是否有其它实时进程对其加以控制。<br>
> * 当中断是实时中断时，会及时响应；如是非实时中断，则由普通Linux处理。<br>
> <br>
> 定时机制的改进：定时器芯片设置为终端计时中断方式，根据最近的进程时间需要而调整。

RT-Linux的特点
> 仅提供了对CPU资源的调度。<br>
> 适合功能简单但有硬实时要求的场合。<br>
> 难于在不同Linux版本之间移植。<br>

## Kurt-Linux
University of Kansas (KU) Real Time Linux，i.e. Kurt-Linux。

Kurt-Linux是在通用Linux系统基础上实现的，可提供微秒级实时操作系统，分为两部分：内核和实时模块。

将系统分为三种状态：正常态、实时态和混合态
1. 在正常态时采用普通的Linux调度策略。
2. 在实时态时只运行实时任务。
3. 在混合态时所有任务都可运行。

通常，为提高Kurt-Linux系统实时特性，需提高时钟精度，但同时也会增加系统负荷，从而降低系统性能。Kurt-Linux的做法是：每次根据实时进程的需要调整Linux系统时钟精度。这样带来的负面作用是需要频繁地对时钟芯片进行编程设置。采用基于时间的静态调度算法，即，实时任务在设计阶段就需要明确地说明实时事件发生的时间。

## RED-Linux
借鉴了RT-Linux的软件模拟中断管理器的机制并提高了时钟中断频率。当收到硬件中断时，RED-Linux中断模拟程序将中断放到一个队列中进行排队，并不立即执行真正的中断处理程序。RED-Linux在Linux内核的很多函数中插入了剥夺点原语，使得进程在内核态时也能被抢占，这提高了内核的实时特性

通过改进实时调度算法提供可支持多种调度算法的通用调度框架，支持三类调度算法：Time-Driven, Priority-Driven, Share-Driven。每个任务需增加如下属性：
1. 任务优先级
2. 任务开始的时间
3. 任务结束的时间
4. 任务结束的时间

RED-Linux的调度程序由两部分组成：
1. Schedule Allocator 任务的初始化，确定其属性
2. Schedule Dispatcher 任务分派

## Raspbian
Raspbian is a free operating system based on Debian optimized for the Raspberry Pi hardware. An operating system is the set of basic programs and utilities that make your Raspberry Pi run. However, Raspbian provides more than a pure OS: it comes with over 35,000 packages, pre-compiled software bundled in a nice format for easy installation on your Raspberry Pi. (不一定需要全部安装)

The initial build of over 35,000 Raspbian packages, optimized for best performance on the Raspberry Pi, was completed in June of 2012. However, Raspbian is still under active development with an emphasis on improving the stability and performance of as many Debian packages as possible.

Raspbian版本演进:Squeeze -> Wheezy -> Jessie -> Stretch -> Buster。常见有完全版(full)和轻量级版(lite)，资源需求差异很大，可根据需要选择


## 选择Linux内核的重要性
The kernel is the central software component of Linux systems. Its capabilities very much dictate the capabilities of the entire system. If the kernel you use fails to support one of your target's hardware components, for instance, this component will be useless as long as this specific kernel runs on your target.

硬件限制(Hardware Constraints)

资源限制(Resources Constraints)

性能限制(Performance Constraints)

## 内核源码树内容

