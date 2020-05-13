# 一、实验目的
* 掌握剪裁Linux内核的方法，理解内核选项的意义。
* 熟悉编译内核并加载内核到目标系统的方法与过程。
* 了解模块与内核的关系，掌握内核模块配置编译、安装与卸载流程，为进一步编程，如驱动编程打下基础。
* 掌握创建、安装（卸载）并使用文件系统的方法。

# 二、实验内容
1. 首先用默认配置重新编译已经安装到开发板（树莓派3B）的内核，因为当前内核是在树莓派官网上直接下载写入树莓派的，没有经过编译过程。用编译后的内核替换现有内核，检查是否通过。
2. 在原始版本基础上，重新配置Linux内核，构建一个嵌入式Linux内核。
3. 编译安装重新配置后的内核、模块及dtbs设备树。
4. 针对默认配置中不少于10个kernel feature进行重新配置（剪裁为主、偶有增加），并解释理由：（Y => N, N => Y）
5. 保留必要的模块安装，剩余（占多数）取消。
6. make后将新的内核安装到开发板运行测试。
7. 选择至少两个模块加载与卸载，检查是否加载、卸载成功。
8. 构建并安装款不同于根文件系统、用于应用开发的其他文件系统（不少于一款）。

# 三、试验过程与结果
> 在试验过程中需要clone git的一些代码，挂北大vpn能几百倍的提速。

## 1.内核编译

Kernel building过程可以在target（树莓派）端或者host（开发机）端进行，这两种方法都可以在[树莓派官方指导](https://www.raspberrypi.org/documentation/linux/kernel/building.md#choosing_sources)中查看具体步骤。

本次实验使用的是host端交叉编译，然后传入树莓派，其操作步骤比target端多，但速度较快。

首先，需要合适的Linux交叉编译主机。倾向于使用Ubuntu。由于Raspbian也是Debian发行版，因此意味着许多方面都相似，例如命令行。

对于大多数win OS的计算机，使用VMware能够模拟Ubuntu OS。

1. 安装所需要的依赖项和交叉编译工具链
```
# 如果在操作过程中提示没有某一个包，按照提示安装即可，如果无法安装，百度搜索
sudo apt install git bc bison flex libssl-dev make libc6-dev libncurses5-dev

# 下载交叉编译工具链
git clone https://github.com/raspberrypi/tools ~/tools

# 将交叉编译器加入系统环境变量，这一步需要根据target的内核来选择交叉编译器，
echo PATH=\$PATH:~/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin >> ~/.bashrc
source ~/.bashrc

# 如果上述的交叉编译器无法使用，使用下列命令选择tools中另一个交叉编译位置
echo PATH=\$PATH:~/tools/arm-bcm2708/arm-linux-gnueabihf/bin >> ~/.bashrc
source ~/.bashrc
```
> 下图为VMware中命令的输入:<br>
![Linux架构目标操作系统1](https://github.com/yiyading/day-read/blob/master/img/Linux%E6%9E%B6%E6%9E%84%E7%9B%AE%E6%A0%87%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F1.png)


2. 下载linux源码并构建源文件和设备文件

```
# 本次实验只需下载当前分支的最小源代码树，使用如下命令
git clone --depth=1 https://github.com/raspberrypi/linux
```
在下载源代码树时，出现如下报错：

![Linux架构目标操作系统2](https://github.com/yiyading/day-read/blob/master/img/Linux%E6%9E%B6%E6%9E%84%E7%9B%AE%E6%A0%87%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F2.png)

上面这种报错的原因是curl的postBuffer的默认值太小，我们需要调整它的大小。我们将postBuffer的值配置成500M，操作步骤和结构如下图所示：

![Linux架构目标操作系统3](https://github.com/yiyading/day-read/blob/master/img/Linux%E6%9E%B6%E6%9E%84%E7%9B%AE%E6%A0%87%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F3.png)

 解决报错后，继续执行下载linux源码操作

![Linux架构目标操作系统4](https://github.com/yiyading/day-read/blob/master/img/Linux%E6%9E%B6%E6%9E%84%E7%9B%AE%E6%A0%87%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F4.png)

```
# 针对不同的树莓派设备，输入以下命令来构建源文件和设备文件
# KERNEL不同是因为不同的设备使用不同的内核
# For Pi 1, Pi Zero, Pi Zero W, or Compute Module 
cd linux
KERNEL=kernel
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcmrpi_defconfig

# For Pi 2, Pi 3, Pi 3+, or Compute Module 3
cd linux
KERNEL=kernel7
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2709_defconfig

# For Raspberry Pi 4
cd linux
KERNEL=kernel7l
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2711_defconfig

# 根据不同树莓派型号选择以上不同的命令构建源文件和设备文件后，执行以下操作
# 该步骤的编译时间可能会比较长，可以使用make -j选项增加线程数，提升速度
# 内核zImage，模块module和设备树dtbs
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules dtbs
```
上述步骤执行第一部后，终端显示<br>
![Linux架构目标操作系统5](https://github.com/yiyading/day-read/blob/master/img/Linux%E6%9E%B6%E6%9E%84%E7%9B%AE%E6%A0%87%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F5.png)

可在linux/arch/arm/boot/目录下查看刚刚编译生成的文件
![Linux架构目标操作系统6](https://github.com/yiyading/day-read/blob/master/img/Linux%E6%9E%B6%E6%9E%84%E7%9B%AE%E6%A0%87%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F6.png)

3. 内核写入SD卡

上面第2步实现了对树莓派内核在host端的交叉编译，使用这种方法的优点在于编译速度较快，因为host端的配置一般都高于树莓派。

我使用的host可thinkpadx1c，可直接插入sd卡。

SD卡需要进行格式化。

插入sd卡后，VMware虚拟机默认无法识别，需要开启“可移动设备”-“Realtek USB3.0-CRW”这一项。

使用lsblk命令查看设备。

VMware未识别sd卡:<br>
![Linux架构目标操作系统7](https://github.com/yiyading/day-read/blob/master/img/Linux%E6%9E%B6%E6%9E%84%E7%9B%AE%E6%A0%87%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F7.png)

VMware识别sd卡：<br>
![Linux架构目标操作系统8](https://github.com/yiyading/day-read/blob/master/img/Linux%E6%9E%B6%E6%9E%84%E7%9B%AE%E6%A0%87%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F8.png)
> sdb1和sdb2即SD卡的分区。

更换SD卡的挂载目录<br>
![Linux架构目标操作系统9](https://github.com/yiyading/day-read/blob/master/img/Linux%E6%9E%B6%E6%9E%84%E7%9B%AE%E6%A0%87%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F9.png)

输入以下命令进行模块安装：
```
cd linux

sudo env PATH=$PATH make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=mnt/ext4 modules_install
```

最后将内核和设备树复制到SD卡上，确保备份旧内核
```
# 下述命令中的#KERNEL替换的是第2步中定义的KERNEL
sudo cp mnt/fat32/$KERNEL.img mnt/fat32/$KERNEL-backup.img
sudo cp arch/arm/boot/zImage mnt/fat32/$KERNEL.img
sudo cp arch/arm/boot/dts/*.dtb mnt/fat32/
sudo cp arch/arm/boot/dts/overlays/*.dtb* mnt/fat32/overlays/
sudo cp arch/arm/boot/dts/overlays/README mnt/fat32/overlays/
sudo umount mnt/fat32
sudo umount mnt/ext4
```

在上述步骤将内核和设备树复制到SD卡中，另一种选择是将内核复制到同一位置，但使用不同的文件名（例如，kernel-myconfig.img），而不是覆盖kernel.img文件。然后通过编辑config.txt文件以选择Pi引导进入的内核：
> kernel=kernel-myconfig.img
这样做的好处是可以使内核与系统和任何自动更新工具管理的内核映像分开，并允许在内核无法启动的情况下轻松地恢复到普通内核。

最后，将SD卡插入树莓派启动。

## 3.内核剪裁
使用make menuconfig指令进入模块选择，剪裁内核

![Linux架构目标操作系统20](https://github.com/yiyading/day-read/blob/master/img/Linux%E6%9E%B6%E6%9E%84%E7%9B%AE%E6%A0%87%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F20.png)

在选择功能时，有以下三个选项
> 1. Y：该功能编译进内核
> 2. N：该功能不编译进内核
> 3. M：该功能形成动态模块，需要时加载到内核

**内核剪裁/增加的本质就是在make内核之前，在make menuconfig选项中，选择make时内核中的模块组成。**

> Gerneral setup
> > 1) <M> Kernel .config support -> < > Kernel .config support，该选项是将.config文件存入内核<br>
> > 2) [\*] Profiling support -> [ ] Profiling support，该选项用于系统评测<br>
> <br>
> Networking suppory
> > 取消Amateur Radio support，IrDA subsystem support，Bluetooth subsystem support，WiMAX support，NFC support等用不上的网络通信模块。

剪裁上述模块之后，重新交叉编译内核，此时再编译内核，只对更改的模块进行编译，因此make的速度会快很多。

make编译之后，进行模块安装，然后将内核和设备树blob复制到SD卡。这些步骤参考上一节的步骤。

然后将SD卡装入树莓派，启动。

**模块的加载与卸载**

远程ssh登录树莓派，可以看到内核已经变为4.19.122-v7+

![Linux架构目标操作系统21](https://github.com/yiyading/day-read/blob/master/img/Linux%E6%9E%B6%E6%9E%84%E7%9B%AE%E6%A0%87%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F21.png)

在内核剪裁时设置为M的模块，不需要绑定内核，可以动态的加载到内核，常用操作指令如下：
```
(1)lsmod(list module,将模块列表显示)，功能是打印出当前内核中已经安装的模块列表。
(2)insmod（install module，安装模块），功能是向当前内核中去安装一个模块，用法是insmod xxx.ko。
(3)modinfo（module information，模块信息），功能是打印出一个内核模块的自带信息。用法是modinfo xxx.ko，注意要加.ko，也就是说是一个静态的文件形式。
(4)rmmod（remove module，卸载模块），功能是从当前内核中卸载一个已经安装了的模块，用法是rmmod xxx.ko  rmmod xxx都可以。
```

在树莓派中输入lsmod命令，却发生了错误显示

![Linux架构目标操作系统22](https://github.com/yiyading/day-read/blob/master/img/Linux%E6%9E%B6%E6%9E%84%E7%9B%AE%E6%A0%87%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F22.png)

## 3.构建NFS文件系统
NFS文件系统由服务端（server）和客户端（client）构成。

树莓派为server，Ubuntu为Client。

server端：
```
# 下载nfs服务器端
sudo apt-get insall ntf-kernel-server

sudo apt-get install portmap

# 建立用于共享的文件夹
sudo mkdir /media/nfs
```

更改配置文件
> sudo nano /etc/exports<br>
> <br>
> 在最后一行写入以下内容:
> /media/nfs \*(rw,sync,no_root_squash)

![Linux架构目标操作系统23](https://github.com/yiyading/day-read/blob/master/img/Linux%E6%9E%B6%E6%9E%84%E7%9B%AE%E6%A0%87%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F23.png)

最后一步：开启nfs服务：
```
sudo /etc/init.d/rpcbing restart

# 执行该步时，如果出现报错，报错原因可能是没有开启nfs服务，重新配置内核编译开启即可。
sudo /etc/init.d/nfs-kernel-server restart
```

client端（VMware中的Ubuntu）：
```
# 下载nfs客户端
sudo apt-get install -y nfs-common

# 建立用于nfs共享的文件夹
sudo mkdir /mnt/nfs
```
我的树莓派和电脑连接的同一个路由器（网段），首先ping通树莓派

![Linux架构目标操作系统24](https://github.com/yiyading/day-read/blob/master/img/Linux%E6%9E%B6%E6%9E%84%E7%9B%AE%E6%A0%87%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F24.png)

将文件夹挂载到服务器端（192.168.0.118）的共享文件下
> sudo mount -t nfs -o nolock -o tcp 192.168.0.118:/media/nfs /mnt/nfs/

执行完挂载，在client端创建文件

![Linux架构目标操作系统27](https://github.com/yiyading/day-read/blob/master/img/Linux%E6%9E%B6%E6%9E%84%E7%9B%AE%E6%A0%87%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F27.png)

在server端可以看到创建的文件

![Linux架构目标操作系统29](https://github.com/yiyading/day-read/blob/master/img/Linux%E6%9E%B6%E6%9E%84%E7%9B%AE%E6%A0%87%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F29.png)

# 四、实验总结
本次实验主要完成以下工作
1. 在host端编译安装了默认配置的内核，并在开发板上完成了测试。
2. 初步掌握对内核进行剪裁的方法。
3. 通过本次实验对host端编译开发板内核有了系统的认识
4. 掌握了安装文件系统的方法。
*  熟练掌握问题解决方法
