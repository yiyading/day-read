# os: ubuntu18.0.4LTS
# kernel:5.3.0

# 一、添加系统调用，显示系统启动以来的缺页次数

缺页中断：cpu访问的页不在主存中，需要操作系统将其调入主存后再进行访问。这个时候，被内存映射的文件实际上成了一个分页交换文件。

## 原理
1.系统每次缺页都会执行缺页中断服务函数do_page_fault，所以可以认为执行该函数的次数就是系统发生缺页的次数。因此，定义一个全局变量pfcount记录此函数执行次数即为系统缺页次数。

2.除了proc系统，用户空间和内核空间的交互还可以由系统调用来完成。通过对pfcount变量添加系统调用，就可以得到系统启动以来的缺页次数。

## 操作步骤
## 1.添加pfconut变量
在解压后的内核文件夹 linux-5.3/include/linux/mm.h 中添加变量声明
```c
// 前两个是文件自带的声明，pfcount是我们自己的声明
extern void * high_memory;
extern int page_clusert;
// extern表明要在外部调用该变量
extern long pfcount;
```

在解压后的内核文件夹 linux-5.3/arch/x86/mm/fault.c 中使用pfcount变量
```c
1493 long pfcount;
1494 static noinline void
1495 __do_page_fault(struct pt_regs *regs, unsigned long hw_error_code, 
1496 		unsigned long address)
```

在do_page_fault函数中加入对pfcount值的更新。
```c
1493 long pfcount;
1494 static noinline void
1495 __do_page_fault(struct pt_regs *regs, unsigned long hw_error_code,
1496                 unsigned long address)
1497 {
1498         pfcount++;
1499         prefetchw(&current->mm->mmap_sem);
1500
1501         if (unlikely(kmmio_fault(regs, address)))
1502                 return;
```

## 2.添加系统调用号
在系统调用表linux-5.3/arch/x86/entry/syscalls/syscall_64.tbl中添加自己的系统调用号
```c
357 433     common  fspick                  __x64_sys_fspick
358 434     common  pidfd_open              __x64_sys_pidfd_open
359 435     common  clone3                  __x64_sys_clone3/ptregs
360 2020    64      pf_count                __x64_sys_pf_count
```

## 3.添加调用函数
在 linux-5.3/kernel/sys.c 中添加系统调用函数，其中SYSCALL_DEFINE0(name)是宏，其定义在/include/linux/syscalls.h文件中，相当于asmlinkage long sys_pf_count(void){...}
```c
196 SYSCALL_DEFINE0(pf_count)
197 {
198         printk("entering syscall pf_count\n");
199         return pfcount;
200 }
```

## 4.编译内核并重启
```
make
make install
make modules_install
```

## 5. 编写c程序并查看结果
```
#include<stdio.h>
#includ<unistd.h>

int main(){
	long pfocunt = syscall(2020);
	printf("pfcount: %ld\n", pfcount);
	return 0;
}
```
上述代码在与 **./src/pfcount.c** 相同，可以直接编译运行

# 二、通过/proc文件系统显示系统缺页次数

## 要求
* 在内核中实现缺页次数统计
* 编译并安装新内核
* 新建内核模块，并加载到新内核，通过/proc实现用户态下查看缺页次数

## 操作步骤
## 1.添加pfcount自变量
在 **linux-5.3/arch/x86/mm/fault.c** 中添加统计缺页次数的pfcount自变量，并将pfcount加入到do_page_fault函数内，用于统计缺页次数
```c
1493 long pfcount;
1494 static noinline void
1495 __do_page_fault(struct pt_regs *regs, unsigned long hw_error_code,
1496                 unsigned long address)
1497 {
1498         pfcount++;
1499         prefetchw(&current->mm->mmap_sem);
1500
1501         if (unlikely(kmmio_fault(regs, address)))
1502                 return;
1503
1504         /* Was the fault on kernel-controlled part of the address space? */
1505         if (unlikely(fault_in_kernel_space(address)))
1506                 do_kern_addr_fault(regs, hw_error_code, address);
1507         else
1508                 do_user_addr_fault(regs, hw_error_code, address);
1509 }
```

## 2.声明全局变量
在 **linux-5.3/includ/linux/mm.h** 头文件中声明全局变量
```c
78 extern void * high_memory;
79 extern int page_cluster;
80 extern long pfcount;
```

## 3.导出pfcount全局变量
在 **linux-5.3/kernel/kallsyms.c** 只有一行添加到处代码
```c
708 static int __init kallsyms_init(void)
709 {
710         proc_create("kallsyms", 0444, NULL, &kallsyms_operations);
711         return 0;
712 }
713 device_initcall(kallsyms_init);
714 EXPORT_SYMBOL(pfcount);
```

## 4.编译内核
```
make mrproper	// 删除原来编译产生的垃圾，第一次编译无需使用
cp /boot/config-4.15.0-55-generic .config	// 导入原系统内核配置
make menuconfig	// 第一次编译可能会报错，按照报错指示进行操作即可
make -j4	// 多线程编译，即4个线程竞争cpu，一般数量为cpu的2倍


```
