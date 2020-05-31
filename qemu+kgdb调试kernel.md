# 一、实验背景
大家在编写程序的时候当遇到问题的时候都会使用调试工具来进行调试，一般都是使用IDE集成的调试工具进行调试。

本次实验将会在Ubuntu上基于kgdb进行Linux内核的调试，需要两个操作系统完成Linux内核的调试，其中被调试操作系统位于虚拟机qemu上。

# 实验目的
搭建Linux内核调试的一整套环境，实现使用kgdb对linux内核的调试。完成对configured变量的watch，并显示出来。
> configured是gdb向内核注册模块中的函数中的一个变量。

# 二、实验原理
## 1.概述
kgdb提供了一种使用gdb调试Linux内核的机制。使用KGDB可以像调试普通的应用程序那样，在内核中进行**设置断点、检查变量值、单步跟踪**程序运行等操作。

使用KGDB调试时需要两台机器，一台作为开发机（DevelopmentMachine）,另一台作为目标机（TargetMachine），两台机器之间通过串口或者以太网口相连。调试过程中，被调试的内核运行在目标机上，gdb调试器运行在开发机上。本次实验的开发机是运行在Ubuntu，在Ubuntu中安装qemu用于运行目标机，之上运行被调试kernel+busybox工具集。

## 2.基本组成
目前内核都集成了kgdb调试环境。gdb远程调试所需要的功能包括**命令处理、陷阱处理及串口通讯**3个主要的部分。kgdb的主要作用是在Linux内核中添加了一个**调试Stub**。调试Stub是Linux内核中的一小段代码，提供了运行gdb的开发机和所调试内核之间的一个媒介。

gdb和调试stub之间通过gdb串行协议进行通讯。**gdb串行协议**是一种基于消息的ASCII码协议，包含了各种调试命令。

## 3.过程描述
当设置断点时，kgdb负责在设置断点的指令前增加一条**trap**指令，当执行到断点时控制权就转移到调试stub中去。此时，调试stub的任务就是使用**gdb串行通信协议**将当前环境传送给gdb，然后从gdb处接受命令。

gdb命令告诉stub下一步该做什么，当stub收到继续执行的命令时，将恢复程序的运行环境，把对CPU的控制权重新交还给内核。

调试示意图如下

![qemu+kgdb1](https://github.com/yiyading/day-read/blob/master/img/qemu%2Bkgdb1.png)

# 三、实验流程
编译内核，要注意在设置config时KGDB是开启的。
> 在执行make之前，通常要执行make menuconfig或其他config命令，下面这张图片如果之前编译时勾选过，就不需要重新编译了，如果没有勾选，则勾选之后重新编译。
![qemu+kgdb2](https://github.com/yiyading/day-read/blob/master/img/qemu%2Bkgdb2.png)

* 以下进行的目录把编译好的内核放在 ~ 目录下进行的。

运行如下命令
```
cd ~
mkdir kDebug
cd kDebug
cp ../linux-5.3/vmlinux .
cp ../linux-5.3/arch/x86/boot/bzImage .
cp /boot/initrd.img-5.3.0
```

## 1.busybox制作根文件系统
BusyBox是一个遵循GPL，以自由软件形式发行的应用程序。Busybox在单一的可执行文件中提供了**精简的Unix工具集**，可运行于多款POSIX环境的操作系统，例如Linux（包括Android）、Hurd、FreeBSD等等。

由于BusyBox可执行文件尺寸小、并通常使用Linux内核，这使得它非常适合使用于嵌入式系统。由于BusyBox功能强大，有些人将BusyBox称为“嵌入式Linux的瑞士军刀”。


// 使用如下命令制作一个文件系统
1. 进入目录kDebug在之下创建一个名为busybox.img,大小为100M的文件，并将其格式化为ext3的文件系统
```
dd if=/dev/zero of=./busybox.img bs=1M count=100
mkfs.ext3 busybox.img
```

2. 将这个虚拟磁盘文件挂载到本地系统中，这样我们可以像访问本地文件一样访问它，并将生成好的busybox的文件拷贝到这个文件里。
```
// busybos-1.31.1后边的1.31.1是版本号
sudo mkdir /mnt/disk
sudo mount -o loop ./busybox.img /mnt/disk
sudo cp -rf ../busybox-1.31.1/busybox /mnt/disk
```

3. 创建必须的文件系统目录
```
cd /mnt/disk/
sudo mkdir dev sys proc etc lib mnt
```

4. 使用busybox默认的设置文件
```
sudo cp -a ~/busybox-1.31.1/examples/bootfloppy/etc/* /mnt/disk/etc
sudo vim /mnt/disk/etc/init.d/rcS
```

5. 将下面内容拷贝到rcS里
```
#! /bin/sh
/bin/mount -a
/bin/mount -t sysfssysfs/sys
/bin/mount -t tmpfstmpfs/dev
#动态添加虚拟机环境中的设备
/sbin/mdev–s
```

6. 做完上面对工作后，我们就可以卸载虚拟磁盘文件了
```
cd ~/kDebug/
sudo umount /mnt/disk
```

## 2.kgdb+qemu调试内核
安装qemu:QEMU是一套由FabriceBellard所编写的**模拟处理器**的自由软件，能模拟至接近真实电脑的速度。
```
sudo apt-get install -y qemu
```

当前还是在~/kDebug目录下，bzImage和inited\*文件之前已经复制过来了。

开始启动
```
sudo qemu-system-x86_64 -S -kernel bzImage -initrd initrd.img-5.3.0 -m 1024

// -m表示非配内存大小为1024Mb, -S表示以调试模式启动，暂定在启动界面；
// -S表示“冷冻”虚拟机，等待调试器发出继续运行命令；
// -kernel表示要调试的内核镜像
注： 这里并没有制作文件系统，大家可以更具自己的需求添加
```

同时摁ctrl+alt+2进入qemu命令行界面，输入**gdbserver tcp::4321**，建立并等待gdb连接
![qemu+kgdb3](https://github.com/yiyading/day-read/blob/master/img/qemu%2Bkgdb3.png)

在kDebug目录下打开另一个终端
```
// 输入如下命令
gdb vmlinux

// 在gdb下输入如下命令
target remote localhost:4321
```
![qemu+kgdb4](https://github.com/yiyading/day-read/blob/master/img/qemu%2Bkgdb4.png)

