对于提供了MMU（存储管理器，辅助操作系统进行内存管理，提供虚实地址转换等硬件支持）的处理器而言，Linux提供了复杂的存储管理系统，使得进程所能访问的内存达到4GB。

内核空间中，从3G到vmalloc_start这段地址是物理内存映射区域（该区域中包含了内核镜像、物理页框表mem_map等等），比如我们使用的VMware虚拟系统内存是160M，那么3G～3G+160M这片内存就应该映射物理内存。在物理内存映射区之后，就是vmalloc区域。

对于160M的系统而言，vmalloc_start位置应在3G+160M附近（在物理内存映射区与vmalloc_start期间还存在一个8M的gap 来防止跃界），vmalloc_end的位置接近4G(最后位置系统会保留一片128k大小的区域用于专用页面映射)

即使用vmalloc申请的内存则位于vmalloc_start～vmalloc_end之间，与物理地址没有简单的转换关系，虽然在逻辑上它们也是连续的，但是在物理上它们不要求连续。

进程的4GB内存空间被人为的分为两个部分--用户空间与内核空间。用户空间地址分布从0到3GB(PAGE_OFFSET，在0x86中它等于0xC0000000)，3GB到4GB为内核空间。

[点我->linux内核空间与用户空间详细介绍](../Linux内核空间与用户空间.md)

kmalloc和get_free_page申请的内存位于物理内存映射区域，而且在物理上也是连续的，它们与真实的物理地址只有一个固定的偏移，因此存在较简单的转换关系，virt_to_phys()可以实现内核虚拟地址转化为物理地址：
```c
// 该转换过程是将虚拟地址减去3G(PAGE_OFFSET=0XC000000)
#define __pa(x) ((unsigned long)(x)-PAGE_OFFSET)
   extern inline unsigned long virt_to_phys(volatile void * address)
   {
       　return __pa(address);
   }

// phys_to_virt从物理地址转化为虚拟地址
 #define __va(x) ((void *)((unsigned long)(x)+PAGE_OFFSET))
   extern inline void * phys_to_virt(unsigned long address)
   {
       　return __va(address);
   }

// virt_to_phys()和phys_to_virt()都定义在include/asm-i386/io.h中。
```

在设备驱动程序中动态开辟内存，不是用malloc，而是kmalloc，或者用get_free_pages直接申请页。释放内存用的是kfree，或free_pages.

## 1.kmalloc() -> kmalloc()分配连续地物理地址，用于小内存分配
kmalloc()内存分配和malloc分配相似，除非被阻塞，否则执行很快，但缺点是不会初始化获得的空间，需要使用memset()函数进行初始化。

kfree()只能用来释放kmalloc()申请的动态空间。不然会导致内核崩溃，出现oops信息。

在内核编程中，可能经常会有一些数据结构需要反复使用和释放，按照通常的思路，可能是使用kmalloc和kfree来实现。但是这种方式效率不高，Linux为我们提供了更加高效的方法——Slab高速缓存管理器。

kmalloc()函数本身是基于slab实现的。slab是为分配小内存提供的一种高效机制。但slab这种分配机制又不是独立的，它本身也是在页分配器的基础上来划分更细粒度的内存供调用者使用。也就是说系统先用页分配器分配以页为最小单位的连续物理地址，然后kmalloc()再在这上面根据调用者的需要进行切分。 

kmalloc()函数原型：
```c
#include<Linux/slab.h>

void *kmalloc(size_t size, int flags);
```

参数说明：
1. size：要分配的块的大小
2. flags：分配标志位，表示分配内存的方法

```
flags的参考用法：
|-进程上下文，可以睡眠		GFP_KERNEL
|-进程上下文，不可睡眠		GFP_ATOMIC
|   |-中断处理程序		GFP_ATOMIC
|   |-软中断			GFP_ATOMIC
|   |-Tasklet			GFP_ATOMIC
|-用于DMA内存，可以睡眠		GFP_DMA | GFP_KERNEL
|-用于DMA内存，不可以睡眠	GPF_DMA | GFP_ATOMIC
```
经常使用的标志：GFP\_KERNEL。表示是这个分配（内部最终通过调用 \_\_get\_free\_pages 来进行, 它是 GFP\_ 前缀的来源）代表运行在内核空间的进程而进行的。

换句话说, 这意味着调用函数是代表一个进程在执行一个系统调用。

使用GFP\_KENRL意味着kmalloc能够使当前进程在少内存的情况下睡眠来等待一页。

因此一个使用GFP\_KERNEL分配内存的的函数是可重入的，并且不能再原子上下文中运行。

**kmalloc()的对应的内存释放函数是kfree**。

## 2.kmalloc、get_free_page和vmalloc的区别

```c
换句话说, 这意味着调用函数是代表一个进程在执行一个系统调用#include <linux/module.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
MODULE_LICENSE("GPL");
unsigned char *pagemem;
unsigned char *kmallocmem;
unsigned char *vmallocmem;


int __init mem_module_init(void){
	// 最好每次内存申请都检查是否成功
	// 下面这段仅仅作为演示代码，没有检查，如需检查，只需添加if判断其返回即可
	pagemem = (unsigned char *)get_free_page(0);
	printk("<1>pagemem addr=%x", pagemem);

	kmallocmem = (unsigned char *)kamlloc(100, 0);
	printk("<1>kmallocmeme addr=%x", kmallocmem);

	vmallocmeme = (unsigned char *)vmalloc(100000);
	printf("<1>vmallocmeme addr=%x", vmallocmem);

	return 0;
}

void __exit mem_module_exit(void){
	free_page(pagemem);
	kfree(kmallocmem);
	vfree(vmallocmem);
}

module_init(mem_module_init);
module_exit(mem_module_exit(;
```

我们的系统上有160MB的内存空间，运行一次上述程序，发现pagemem的地址在0xc7997000（约3G+121M）、kmallocmem 地址在0xc9bc1380（约3G+155M）、vmallocmem的地址在0xcabeb000（约3G+171M）处，符合前文所述的内存布局。
