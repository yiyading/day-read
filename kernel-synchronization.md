OS:ubuntu18.0.4.LTS
kernel:5.3
# 一、kernel synchronization

## 1.用户空间的同步
对于用户空间的应用程序开发，用户空间的任务调度与同步之间的关系相对简单，无需过多考虑需要同步的原因。主要是因为：
1. 在用户空间中各个进程都拥有**独立的运行空间**，进程**内部的数据对外不可见**，所以各个进程即使并发执行也不会产生对数据访问的竞争。
2. 用户空间与内核空间独立

用户同步仅仅需要在**进程间通讯**和**多线程编程**时需要考虑。

## 2.内核空间的同步
如果多个**内核任务**同时访问或操作**共享数据**，可能发生各任务之间相互覆盖共享数据的情况，造成被访问数据处于不一致状态。

并发访问共享数据是造成系统不稳定的一个隐患，**这种错误难于跟踪和调试**。】】


## 3.内核同步介绍
内核中很多数据都是共享资源。

对共享数据的访问须遵循一定的访问规则，防止并发访问。

2.0以后内核支持对称多处理器（SMP）
1. 内核代码同时运行在两个或多个处理器上
2. 不同处理器的内核代码同一时刻，并发访问某些共享资源。

2.6内核已经发展为抢占式内核。调度程序可以在任何时刻抢占正在运行的内核代码。重新调度其他的进程执行

## 4.基本概念
临界资源：一次进允许一个进程使用的共享资源，诸多进程采用互斥方式，实现对这种资源的共享
> 属于临界资源的硬件：打印机，磁带机等

> 属于临界资源的软件：消息队列，变量，数组，缓冲区等。

临界区：每个进程中访问临界资源的那段代码成为**临界区**。每次只允许一个进程进入临界区，进入后，不允许其他进程进入
> 在临界区要避免并发访问

## 5.加锁

如果我们需要保护的共享资源是复杂的数据，如链表；**确保一次只能有一个线程对数据结构进行操作**

**锁**提供了这种保护机制

要给**数据**加锁，而不是给代码加锁

需要加锁的数据：
1. 可以被其他执行线程访问的数据
2. 任何其他东西能够看到它

几乎访问所有**内核全局变量**和**共享数据**都需要某种形式的同步方法

## 6.什么造成并发
中断：中断可能随时打断当前正在执行的代码。

软中断与tasklet：内核能在任何时候唤醒或调度软中断和tasklet，打断当前正在执行的代码。

内核抢占：内核具有抢占性，内核的任务可能会被另一个任务抢占。

睡眠：在内核执行的进程可能会睡眠，这就唤醒调度程序，从而导致调度一个新的用户线程执行。

对称多处理：两个或多个处理器可以同时执行代码。

## 7.死锁
线程无法获得自己所需要的资源，一直在等待

条件：有一个或多个执行线程和一个或多个资源，**每个线程都在等待其中的一个资源，但所有的资源都已经被占用，所有线程互相等待，但它们永远不会释放已经占有的资源**。任何线程都无法继续，这样便产生了死锁。

产生死锁的四个必要条件：
1. 互斥条件
2. 不可剥夺条件
3. 请求与保持条件
4. 循环等待条件

自死锁：一个线程**试图去获得一个自己已经持有**的资源，它将不得不等待锁被释放，最终产生死锁。

经典问题————哲学家进餐问题
> 5个哲学家在同一张桌子上就餐。桌子上有5只碗和5只筷子，其中，筷子是共享资源。哲学家可能思考，也可能就餐。哲学家就餐时，只能使用左右最靠近他的筷子，且只有同时获得2只筷子才能就餐。用餐完毕，释放筷子<br>
> <br>
> 可能的死锁：5个哲学家同时拿起左面的筷子，就无法再获得右面的筷子<br>
> <br>
> <br>
> 解决办法
> 1. 最多允许4位拿起左面的筷子
> 2. 仅当能同时获得左右2只筷子时，才允许拿起筷
> 3. 规定奇数号的哲学家先拿左面的筷子，后拿右面的筷子，而偶数号哲学家拿筷子的顺序则相反。

# 二、kernel synchronization methods
主流Linux内核中包含了几乎所有现代的操作系统具有的同步机制，这些同步机构包括
```
Per-CPU variables
Atomic operations
semaphore
rw_semaphore
spinlock
Rwlock
seqlock
BKL(Big Kernel Lock)
Brlock（Big Reader Lock ）（只包含在2.4内核中）
RCU（Read-Copy Update）
Completions
Memory Barriers
Local Interrupt Disabling
Disabling and Enabling Deferrable Functions
```

## 2.atomic operation
原子操作保证**指令以原子的方式被执行，执行过程不被打断**。

Linux内核提供了两组原子操作接口
1. 一组对整数进行操作
2. 一组针对单独的位进行操作

### 原子整数操作
Linux内核提供了一个专门的atomic_t类型（一个原子访问计数器）和一些专门的函数，这些函数作用于atomic_t类型的变量。atomic_t类型定义在文件</include/linux/types.h>中：
```c
typedef struct{
	int counter;
} atomic_t;
```

atomic_tl类型：
1. 原子操作函数只接受atomic_t类型的操作数
2. 编译器不对相应的值进行访问优化

原子整数操作的使用：
1. 常见的用途是计数器，计数器无需复杂的锁机制
2. 能使用原子操作的地方，尽量不要使用复杂的锁机制。

举个栗子：
```c
// 定义一个atomic_t类型的数据很简单，还可以定义时给它设初值
atomic_t u;			/*定义u*/
atomic_t v = ATOMIC_INIT(0);	/*定义v并把它初始化为0*/

// 对其操作如下
atomic_set(&v, 4);	/*v=4(原子地)*/
atomic_add(2, &v);	/*v=v+2=6(原子地)*/
atomic_inc(&v);		/*v=v+1=7（原子地)*/

// 如果需要将atomic_t转换成int型，可以使用atomic_read()来完成
printk("%d\n", atomic_read(&v);		//打印7
```

### 原子位操作
1. 操作函数的参数是**一个指针**和**一个位号**，**第0位**是给定地址的**最低有效位**
2. 原子位操作中**没有特殊的数据类型**，例如：set_bit(0, &word);

原子位操作函数
```c
viod set_bit(int nr,void *addr)			//原子地设置addr所指对象的第nr位
viod clear_bit(int nr,void *addr)		//原子地清空addr所指对象的第nr位
viod change_bit(int nr,void *addr)		//原子地翻转addr所指对象的第nr位
int test_and_set_bit(int nr,void *addr)		//原子地设置addr所指对象的第nr位，并返回原先的值
int test_and_clear_bit(int nr,void *addr)	//原子地清空addr所指对象的第nr位，并返回原先的值
int test_and_change_bit(int nr,void *addr)	//原子地翻转addr所指对象的第nr位，并返回原先的值
int test_bit(int nr,void *addr)			//原子地返回addr所指对象的第nr位
```

## 3.Spin lock（自旋锁）
linux中最常见的锁：自旋锁
1. 自旋锁最多只能被一个可执行线程持有。
2. 原子的test-and-set。
3. 如果一个线程试图获得一个被争用的自旋锁，那么该线程就会一直进行**忙循环—旋转—等待锁重新可用**。
4. 锁未被争用，请求锁的执行线程便立即得到它，继续执行。

自旋锁不应该被长时间持有，因为被征用的自旋锁使得请求它的线程在等待锁重新可用时**自旋**，浪费处理器时间。

自旋锁被定义在</include/linux/spinlock\_types.h>
```c
12 #if defined(CONFIG_SMP)
13 # include <asm/spinlock_types.h>
14 #else
15 # include <linux/spinlock_types_up.h>
16 #endif

// include<asm/spinlock_types.h>中定义了arch_spinlock_t
typedef struct arch_spinlock{
	union{
		__ticketpair_t head_tail;
		struct __raw_tickets{
			__ticket_t head, tail;
		}tickets;
	};
}arch_spinlock_t;

// include<linux/spinlock_types_up.h>定义了arch
```


