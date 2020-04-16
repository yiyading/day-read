# 添加系统调用，显示系统启动以来的缺页次数

缺页中断：cpu访问的页不在主存中，需要操作系统将其调入主存后再进行访问。这个时候，被内存映射的文件实际上成了一个分页交换文件。

# 原理
1.系统每次缺页都会执行缺页中断服务函数do_page_fault，所以可以认为执行该函数的次数就是系统发生缺页的次数。因此，定义一个全局变量pfcount记录此函数执行次数即为系统缺页次数。

2.除了proc系统，用户空间和内核空间的交互还可以由系统调用来完成。通过对pfcount变量添加系统调用，就可以得到系统启动以来的缺页次数。

# 操作步骤
## 1.添加pfconut变量
在解压后的内核文件夹 linux-version_number/include/linux/mm.h 中添加变量声明
```c
// 前两个是文件自带的声明，pfcount是我们自己的声明
extern void * high_memory;
extern int page_clusert;
// extern表明要在外部调用该变量
extern long pfcount;
```

在解压后的内核文件夹 linux-version_number/arch/x86/mm/fault.c 中使用pfcount变量
```c
1493 long pfcount;
1494 static noinline void
1495 __do_page_fault(struct pt_regs *regs, unsigned long hw_error_code, 
1496 		unsigned long address)
```

在do_page_fault函数中加入对pfcount值得更新。
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
