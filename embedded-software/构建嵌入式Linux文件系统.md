# 全文结构
文件系统的特性 -> 文件系统的载体 -> 虚拟文件系统 -> 根文件系统的构建 -> 文件系统的安装与卸载

# 一、文件系统的特性
## 1.什么是文件系统
文件系统的定义：操作系统中处理文件的部分称为文件系统。是统一管理信息资源的一种软件，负责管理文件存储、检索、更新等，提供安全可靠的共享和保护手段，且方便用户使用。

文件系统的功能：文件的构造、命名、存取、共享、保护等。

文件系统的存储媒质：软盘、硬盘、光盘、FLASH、RAM、ROM、MMC、SD、TF、网络等。

Characterizing File Systems
> Online write support 支持在线写入
> > 在线写入包括增、删、改.<br>
> > 多数文件系统支持在线更新.<br>
> > 也有部分文件系统不支持在线更新，这使得文件系统结构更简单.<br>
> > 有些很少发生改变的数据不需要这一特性支持.<br>
> > 例子：Ext2， Ext3，Ext4，Tmpfs ，JFFS2，YAFFS2
> <br>
> Persistent 持久性
> > 多数文件系统都具有持久性，但并非所有数据都值得持久保存，比如临时数据，/proc，/sys中的数据.<br>
> > 某些文件系统会驻留在RAM中，使得对文件的操作更方便、快捷，系统重启这些数据会丢失，所以应预先知道重启不会对数据构成损害.<br>
> > 例子：Ext2 ，JFFS2，YAFFS2<br>
> <br>
> Power down reliability 断电可靠性
> > 断电可靠性是指当发生断电或者电池电能用光，系统能否恢复之前的重要数据，多数嵌入式设备，不提供事先设计好的系统工具来正常关机，甚至有些设备没有电源开关，经常直接拔插电源或者电池。这使得系统设计更简单。为了使数据具有断电可靠性，文件系统大都采用基于日志的设计方法，使数据损失最小化.<br>
> > 例子：Ext3，Ext4，JFFS2，YAFFS2
> <br>
> Compression 可压缩<br>
> > 一个文件系统可以自动压缩部分或全部数据和元数据，然后再把它们写入到存储设备，从存储设备读回来时再自动解压缩，这些应该是文件系统自动完成的，是否采用可压缩的文件系统取决于系统需求，比如存储容量。压缩解压缩过程总是需要耗费一些CPU时间，这在多大程度上影响到实时性，也是需要设计者考量的。但经优化设计的文件系统，这一影响应该是非常有限的.<br>
> > 例子：Cramfs，JFFS2

## 2.Ext2
Ext2 is one of the earliest file systems to be used with Linux and is still a very suitable choice when used with any of the following:
1. RAM disks in read/write mode.
2. CompactFlash devices in IDE mode.
3. NAND or NOR flash in read-only (or mostly read-only) mode, implemented either via MTD subsystem raw block device emulation or with an FTL/NFTL translation layer.

NFTL:
1. NAND Flash Translation Layer (NAND Flash 转换层，NFTL），在NAND Flash芯片上实现了一个虚拟块设备。Ext2通过NFTL管理的MTD设备存取数据.



# 三、虚拟文件系统
## 1.LinuxVirtual File System（VFS）
不同的文件要使用不同的文件系统，VFS提供了一种API，可对不同文件系统的文件进行操作。
1. 提供对不同文件系统的封装，提供统一的文件操作界面。
2. 当系统需要挂载多种文件系统时，方便编程。
3. 虚拟文件系统本身不含有文件，也不直接管理文件，不需要保存在永久介质中，VFS是系统启动时由内核在内存中创建的。

file_operations结构提供各种文件的操作与管理接口函数。<br>
![8.1]()

举个例子：VFS role in a simple file copy operation
![8.2]()

Linux VFS objects types：
1. superblock object
> Stores infor concerning a mounted file system.
> <br>
> For disk-based fs, usually corresponds to a fs control block stored on disk.

2. inode object
> Stores general infor about a specific file.
> <br>
> For disk-based fs, usually corresponds to a file control block stored on disk.

3. file object
> Stores infor about the interaction between an open file and a process.
> <br>
> Exists only in kernel memory during the period when a process has the file open.
4. dentry object
>Stores infor about Linking of a directory entry with the corresponding file.
> <br>
> Each disk-based fs stores this infor in its own particular way on disk.

VFS在文件结构中为用户提供了**统一的操作界面** => 由文件结构中指针f_op指向的file_operations结构提供，进程在引用file_operations结构中的指针时，将跳转到相对应的操作函数上，如果文件系统中没有file_operations结构中对应的操作， 指针将指向NULL

VFS系统调用常用如：open()、read()、write()、close()、ioctl()
![8.3]()

file_opreations结构：支持所有的文件系统，应用时根据情况选择几个即可
```c
// 定义位于include/linux/fs.h
struct file_operations {
	struct module *owner;
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*aio_read) (struct kiocb *, char __user *, size_t, loff_t);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	ssize_t (*aio_write) (struct kiocb *, const char __user *, size_t, loff_t);
	int (*readdir) (struct file *, void *, filldir_t);
	unsigned int (*poll) (struct file *, struct poll_table_struct *);
	int (*ioctl) (struct inode *, struct file *, unsigned int, unsigned long);
	int (*mmap) (struct file *, struct vm_area_struct *);
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, struct dentry *, int datasync);
	int (*aio_fsync) (struct kiocb *, int datasync);
	int (*fasync) (int, struct file *, int);
	int (*lock) (struct file *, int, struct file_lock *);
	ssize_t (*readv) (struct file *, const struct iovec *, unsigned long,loff_t *);
	ssize_t (*writev) (struct file *, const struct iovec *, unsigned long, loff_t *);
	ssize_t (*sendfile) (struct file *, loff_t *, size_t, read_actor_t,void __user *);
	ssize_t (*sendpage) (struct file *, struct page *, int, size_t,loff_t *, int);
	unsigned long (*get_unmapped_area) (struct file *, unsigned long,unsigned long, unsigned long, unsigned long);
}；
```

## 2.File systems supported by VFS
**只有部分文件系统适合于嵌入式系统开发！**
Disk-based file systems:
1. Cramfs, Ext2, Ext3, Ext4, ReiserFS, JFFS2, YAFFS2, Romfs
2. sysv filesystem (System V, Coherent, Xenix), UFS (BSD , Solaris , NEXTSTEP), MINIX filesystem, and VERITAS VxFS (SCO UnixWare)
3. MS-DOS, VFAT (Windows 95 and later releases), and NTFS (Windows NT 4 and later releases)
4. ISO9660 CD-ROM filesystem and Universal Disk Format (UDF ) DVD filesystem.
5. IBM's OS/2 (HPFS ), Apple's Macintosh (HFS ), Amiga's Fast Filesystem (AFFS ), and Acorn Disk Filing System (ADFS)
6. Many Others

Network file systems provide easy access to files in other networked computers.

Some well-known network file systems supported by the VFS are:
1. NFS (Network File System)
2. Coda (Constant Data Availability, CMU since 1987 )
3. AFS (Andrew filesystem, CMU)
4. CIFS (Common Internet File System, used in Windows )
5. NCP (Novell's NetWare Core Protocol)

# 四、根文件系统的构建
## 1.根文件系统的构建与移植
内核启动之后的运行需要根文件系统的支持。Linux内核的许多服务和设备管理都是以文件接口的方式来提供的。“内核＋库＋应用程序”是Linux系统运作的基本方式，而这强烈依赖于文件系统:
1. 库和应用程序都需要存放在一个文件系统中
2. 系统库和系统程序一般存放在根文件系统中
3. 根据bootloader的启动模式不同，需要预先将Kernel Image加载到根文件系统启动目录(rootfs/boot)中，如果升级内核，需要替换此处的Kernel Image

根文件系统是Linux系统的核心部分，除了包含系统使用的软件和库，以及支持用户编程的应用软件，还被用来作为存储中间数据的区域。在Linux系统启动时，首先完成内核安装及环境初始化，最后内核会寻找一个文件系统作为根文件系统被加载，**Linux系统中使用“/”来唯一表示根文件系统的安装路径。其它文件系统都会挂载在根目录下。**

嵌入式系统中可以选择的根文件系统有：Romfs、CRAMFS、YAFFS、TMPFS、JFFS2、EXT2、EXT3、EXT4 等，**甚至可以使用NFS**

Root filesystem top-level directories：对于嵌入式系统，有些目录是必须的，但有些目录不是必须的。
![8.4]()

Tailoring root filesystem：The "official" rules to build a root filesystem are contained in the Filesystem Hierarchy Standard (FHS). Some directories, such as /home, /mnt, /opt, and /root, are only used in multiuser systems. If there are no users/administrators, these directories may not be installed. Depending on the bootloader and its configuration, you may not need to have a /boot directory. The remaining directories **/bin, /dev, /etc, /lib, /proc, /sbin, and /usr, are essential.**

## 2.根文件系统的内容
/bin：Binaries that are essential to both users and system administrators，这是通用应用程序存放目录，比如ls、cat、mkdir等。也可能包括功能强大的命令工具集busybox。Most nonessential user binaries are located in /usr/bin，**必选**

/sbin：Binaries that are essential to system administration, but will never be used by ordinary users，这是系统管理员服务程序，其中最重要的就是供内核初始化之后执行的/sbin/init进程。系统启动时由init解释并运行/etc/inittab，inittab会指导init去调用一个最重要的系统初始化程序/etc/init.d/rcS。Most nonessential system administration tools are located in /usr/sbin，**必选**

/etc：系统配置文件、启动文件及用户数据存放目录。目录下所有内容是在内核运行后，由/mnt/etc 拷贝得到，所以开始时为空目录。系统启动时，把它配置为ramfs，即位于RAM 地址空间中，所以是可读写的目录，**必选**

/boot：bootloader用到的静态文件。如果bootloader被设置成能够从根文件系统启动内核，需要将内核映像(kernel image)复制/boot。内核配置文件 .config，**可选**

/usr：用于存放用户程序和配置文件的目录，可以根据需要进行设置，**可选**
> /usr/etc/rc.local：被init.d/rcS文件调用执行的特殊文件，与Linux系统硬件平台相关，如安装内核模块、进行网络配置、运行应用程序、启动图形界面等。<br>
> <br>
> /usr/etc/profile：执行该文件配置应用程序需要的环境变量等。<br>


