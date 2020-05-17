# 一、文件系统的特性
Linux中一切皆文件！

什么是文件系统？
> 1. 在OS（操作系统）中，负责处理文件的部分称为文件系统。
> 2. 文件系统的功能：文件的构造、命名、存取、共享、保护等。
> 3. 文件系统的存储介质：FLASH、MMC、SD、TF等。

**Linux支持的FS相当多，但用于嵌入式的只有若干种。**

FS（文件系统）的特性：
> 1. Online write support 支持在线更新
> 2. Persistent 持久性：多数FS都具有持久性，但并非所有data都值得持久保存。例如临时数据/proc，/sys中的数据。
> 3. Power down reliability 断电可靠性
> 4. Compression 可压缩：对数据进行压缩

## 1.Ext2
The second extended file system(Ext2).

特性：
> 1. Writable and persistent
> 2. **No power-down reliability**
> 3. **No compression**
> 4. Supports up to 16 or 32 TB of storage.
> 5. Built for use with block devices.（后文Ext2的物理结构使用Block性质）

Ext3 adds reliability via journaling（日志）.

Ext4 after 2.628，在Ext3基础上增加了新特性，如延迟分配和在线碎片整理等。

Ext2的物理结构如下图所示：
![嵌入式Linux文件系统1](https://github.com/yiyading/day-read/blob/master/img/%E5%B5%8C%E5%85%A5%E5%BC%8FLinux%E6%96%87%E4%BB%B6%E7%B3%BB%E7%BB%9F1.png)

* Boot Block：文件系统引导块
* Super Block：每个块组都有内容相同的超级块，是文件系统控制块
* Group Descriptors：描述本块组位图、inode位图存放位置等
* Data block Bitmap：记录数据块使用情况，1被占用，0空闲
* inode Bitmap：记录每个inode使用情况，1被占用，0空闲
* inode Table：记录每个inode占用的若干个数据块
* Data blocks：存放实际数据


## 2.Ext3 -> Ext4
Ext开头的文件系统，是一个系列，随着数字的增加，FS更加优化。

从Ext3到Ext4进行了大量的优化，下边只记录两种：
1. 更大的FS：16TB -> 1024\*1024TB
2. 更大的文件：2TB -> 16TB

更多Ext系列文件系统的特性，请找度娘。

## 3.other
**1. Cramfs**: A very simple, compressed and read-only filesystem **aimed at embedded systems** with any block device.

**2. Tmpfs**: Tmpfs is a filesystem stored in virtual memory. It's writable but not persistent.

Tmpfs能够根据其存储的内容**扩大或者缩小**，因此它在存储**临时文件**时非常有用。

**3. JFFS2**：Journalling Flash File System Version2（闪存日志型文件系统第2版）

JFFS2支持**writable, Persistent, compressed, power-down reliable**。

**4. YAFFS2**：Yet Another Flash File System Version2。

YAFFS2支持**writable, persistent, power-down reliable**。

## 4.基于FLASH的文件系统
Flash(闪存)作为嵌入式系统的主要存储媒介，有其自身的特性。Flash的写入操作只能把对应位置的1修改为0，而不能把0修改为1(擦除Flash就是把对应存储块的内容恢复为1)，因此，一般情况下，向Flash写入内容时，需要先擦除对应的存储区间，**这种擦除是以块(block)为单位进行的。**

闪存主要有NOR和NAND两种技术。Flash存储器的擦写次数是有限的，NAND闪存还有特殊的硬件接口和读写时序。因此，必须针对Flash的硬件特性设计符合应用要求的文件系统；传统的文件系统如ext2等，用作Flash的文件系统会有诸多弊端。

**常用的有JFFS2，YAFFS2，Cramfs**

## 5.基于RAM的文件系统

Tmpfs

# 二、文件系统的载体

嵌入式存储设备：嵌入式系统中使用的存储设备与工作站和服务器不同，倾向于使用固态存储设备（如flash芯片和flash磁盘），这使得操作他们的工具（例如分区、复制、擦除）也有所不同。

## 1.Memory Technology Devices（MTD）
Linux中，MTDs设备包括传统的ROM以及当今的NOR/NAND flash，他们有不同的功能、特性和限制：
> 1. 一些flash可以直接映射内存（NOR flash）
> 2. NAND flash没有直接的从内存映射，kernel须使用bounce buffers将数据从flash复制到RAM中来访问flash中的数据，NAND flash采用的是以此读取一个数据块的技术。但NAND flash的优点是：相对便宜，存储量大，但不能直接运行其上的代码。

为了在系统中编程和使用MTD设备，嵌入式系统开发人员通常必须使用专用于这类设备的工具和方法。

为了避免不同技术提供不同接口，Linux kernel提供了MTD子系统统一层，即调用接口。

内核中的软件模块（也叫用户模块），其上层接口于底层MTD芯片驱动程序无缝结合，并且他通过内核上层，或者某些情况向用户空间提供接口，可用来访问地城MTD芯片驱动程序。

软件模块本质上就是一个上层与底层的中介，作用是整合不同的技术于一个接口。

可通过kernel配置、编译过程启动。

## 2.Non-MTD Flash-Based devices
除了基于MTD的闪存设备，越来越多的嵌入式系统使用包括CompactFlash、SD和所有流行的U盘等其他存储设备。

这些设备都预先打好包并设置为磁盘设备，装有常规的文件系统（如FAT16或FAT32），Linux对这些附加flash设备的支持非常好，内核为此内置了通用的USB存储驱动程序。

## 3.other
Native CFI Flash：可通过Kernel配置、编译过程启动。

DiskOnChip（DOC）：可通过Kernel配置、编译过程启动。

# 三、虚拟文件系统（Linux Virtual File System）
1. VFS提供对不同文件系统的封装，提供统一的文件操作界面。
2. 当系统需要挂载多种文件系统时，方便编程。
3. VFS本身不含有文件，也不管理文件，不需要保存在永久介质中，VFS是系统启动时由内核在内存中创建。

file_operations结构提供各种文件的操作与管理接口：

![嵌入式Linux文件系统2](https://github.com/yiyading/day-read/blob/master/img/%E5%B5%8C%E5%85%A5%E5%BC%8FLinux%E6%96%87%E4%BB%B6%E7%B3%BB%E7%BB%9F2.png)

下图展示了一个在Ext2和MS-DOS文件系统间的copy operation

![嵌入式Linux文件系统3](https://github.com/yiyading/day-read/blob/master/img/%E5%B5%8C%E5%85%A5%E5%BC%8FLinux%E6%96%87%E4%BB%B6%E7%B3%BB%E7%BB%9F3.png)

Linux VFS objects types
> 1. The superblock object：存放相关被挂载的文件系统的信息
> 2. The inode object：存放特定文件的一般信息
> 3. The file object：存放打开的文件和进程之间的信息
> 4. The dentry object：存放相关文件直接入口的链接信息

VFS通过文件结构中指针f_op指向**file****_****operations**结构，来为用户提供统一的操作界面。

进程在引用file_operations结构中的指针时，将跳到对应的文件系统的操作函数上，如果没有该文件系统，则指针指向NULL。

VFS系统调用：常用如open(), read(), write(), close()

![嵌入式Linux文件系统4](https://github.com/yiyading/day-read/blob/master/img/%E5%B5%8C%E5%85%A5%E5%BC%8FLinux%E6%96%87%E4%BB%B6%E7%B3%BB%E7%BB%9F4.png)

VFS支持的文件系统：
1. 磁盘文件系统（Disk-base file systems）：Cramfs, Ext2, Ext3, Ext4, ReiserFS, JFFS2, YAFFS2, Romfs

* 注：只有部分文件系统适合嵌入式系统开发

2. Network file systems（NFS）提供了一种获得其他相关联计算机中文件的简单方法。VFS支持的NFS文件系统：
> 1. NFS (Network File System)**特别适用于嵌入式系统早期开发调试阶段**
> 2. Coda (Constant Data Availability, CMU since 1987 )
> 3. AFS (Andrew filesystem, CMU)
> 4. CIFS (Common Internet File System, used in Windows )
> 5. NCP (Novell's NetWare Core Protocol)

# 四、根文件系统的构建
内核启动之后的运行需要根文件系统的支持。

Linux系统启动后，首先完成Kernel安装及环境初始化，最后kernel会寻找一个文件系统作为根文件系统挂载在根目录下。

**Linux系统中使用“/"来表示根文件系统的安装路径。其他文件系统都会挂载在根目录下**。根文件系统的可选项非常多。

在剪裁根文件系统时，有一些目录是必须的：
> 1. /bin
> > Binaries that are essential to both users and system administrators.<br>
> > 通用应用程序存放目录，比如ls、cat、mkdir等。也可能包括功能强大的命令工具集busybox。<br>
> > Most nonessential user binaries are located in /usr/bin.<Br>
> > 必选
> 2. /sbin
> > Binaries that are essential to system administration, but will never be used by ordinary users.<br>
> > 系统管理员服务程序，其中最重要的就是供内核初始化之后执行的/sbin/init进程。系统启动时由init解释并运行/etc/inittab，inittab会指导init去调用一个最重要的系统初始化程序/etc/init.d/rcS。<br>
> > Most nonessential system administration tools are located in /usr/sbin.<br>
> > 必选<br>
> 3. /lib
> > The libraries required to boot the system and run the most essential commands.<br>
> > 存放程序运行所需要的动态库、静态库。<br>
> > 内核模块 (以驱动模块为主)。<br>
> > 必选
> 4. /proc：必选
> 5. /dev：必选
> 6. /etc：必选
> 7. /boot：必选
> 8. /mnt：必选
> 9. /usr：必选

Busybox通过一个小于1 MB(与glibc静态链接)或小于500 KB(与uClibc静态连接)的单一可执行文件实现大多数Unix命令。

# 五、文件系统的安装与卸载

文件系统可选项较多，如果想要安装某个文件系统，在百度上可以找到丰富的资源。

简介NFS：NFS是一种分布式文件系统。提供NFS服务的一方是NFS Server，使用NFS服务的一方是NFS Client。Client可以将Server提供的共享目录挂载到本地目录上，在本地操作如同对Server操作。

在开发板和主机之间使用NFS，可以更加便捷的完成对程序的开发，减少image的下载次数和对flash的烧写次数。缩短开发周期。
