- [ 一、什么是命名管道（FIFO）](#一、什么是命名管道（FIFO）)
- [二、创建命名管道](#二、创建命名管道)
- 三、访问命名管道
    - 1.打开FIFO文件
    - 2.open调用阻塞
- 四、使用FIFO实现进程间通信
- 五、命名管道安全问题
- [六、FIFO与匿名管道对比](#六、FIFO与匿名管道对比)
# 一、什么是命名管道（FIFO）
命名管道也称为FIFO文件，这是一类特殊类型的文件，它在文件系统中以文件名的形式存在，但它和有名管道的区别在于能让两个没有关系的进程进行通信。

Linux中一切皆文件，因此对FIFO的操作和一般文件的操作非常统一，可以像普通的文件名一样在命令中使用。

# 二、创建命名管道
```c
#include <sys/types.h>
#include <sys/stat.h>
int mkfifo(const char *filename, mode_t mode);
int mknod(const char *filename, mode_t mode | S_IFIFO, (dev_t)0);
```
这两个函数都能创建一个FIFO文件，filename指定了文件名（**使用文件所在的绝对路径**），而mode指定了文件的读写权限。<br>

mknod是比较老的函数，而**使用mkfifo函数更加规范**，尽量使用mkfifo而不是mknod。<br>

# 三、访问命名管道
## 1.打开FIFO文件
与打开其他文件一样，FIFO文件也可以使用open来调用打开。但要注意与普通文件在打开上的区别：
> 1.FIFO**不能**以O_RDWR（读/写）模式打开，因为一个管道以读/写方式打开，进程会读回自己的输出，而我们使用FIFO通常是单向的数据传输。<br>
> <br>
> 2. 传递给open的是FIFO的路径名，不是正常的文件。<br>

打开FIFO文件通常有四种方式：<br>
> open(const char \*path, O_RDONLY); // 1 <br>
> open(const char \*path, O_RDONLY | O_NONBLOCK); // 2 <br>
> 
> open(const char \*path, O_WRONLY); // 3 <br>
> open(const char \*path, O_WRONLY | O_NONBLOCK); // 4 <br>

open函数中的第二个参数中O_NONBLOCK选项表示非阻塞；如果没有O_NONBLOCK选项，表示open是阻塞的。<br>

## 2.open调用阻塞
**对于读和写，非阻塞的结果不同**。<br>

对于以只读方式（O_RDONLY）打开的FIFO文件，如果open调用是阻塞的（即第二个参数为O_RDONLY），除非有一个进程以写方式打开同一个FIFO，否则它不会返回；如果open调用是非阻塞的的（即第二个参数为O_RDONLY | O_NONBLOCK），则即使没有其他进程以写方式打开同一个FIFO文件，open调用将成功并立即返回文件标识符。

对于以只写方式（O_WRONLY）打开的FIFO文件，如果open调用是阻塞的（即第二个参数为O_WRONLY），open调用将被阻塞，直到有一个进程以只读方式打开同一个FIFO文件为止；如果open调用是非阻塞的（即第二个参数为O_WRONLY | O_NONBLOCK），open总会立即返回，但如果没有其他进程以只读方式打开同一个FIFO文件，open调用将返回-1，并且FIFO也不会被打开。

# 四、使用FIFO实现进程间通信
例子为同时打开读写两个客户端，在“读”代码中创建FIFO文件，实时的将写入读出。<br>

如果想要通过FIFO从一个单独的文件中读出数据，并写入另外一个文件中，只需在代码中加入打开/关闭新文件和读出/写入代码段即可。

1. 阻塞通信
> [readfifo.c](https://github.com/yiyading/day-read/blob/master/FIFO%E9%80%9A%E4%BF%A1/readfifo.c)<br>
> <br>
> [writefifo.c](https://github.com/yiyading/day-read/blob/master/FIFO%E9%80%9A%E4%BF%A1/writefifo.c)<br>

2. 非阻塞通信
> [readfifo_NONBLOCK.c](https://github.com/yiyading/day-read/blob/master/FIFO%E9%80%9A%E4%BF%A1/readfifo_NONBLOCK.c)<br>
> <br>
> [writefifo_NONBLOCK.c](https://github.com/yiyading/day-read/blob/master/FIFO%E9%80%9A%E4%BF%A1/writefifo_NONBLOCK.c)<br>

# 五、命名管道的安全问题
FIFO通信就是一个进程向FIFO文件中写数据，而另一个进程则在FIFO文件中读数据。

如果只使用一个FIFO文件，但同时有多个进程向FIFO文件中写数据，而只有一个进程从FIFO文件中读数据，会出现数据块的交错。

不同进程向同一个FIFO文件中写入数据是非常常见的。为了解决这一问题，可以让写操作原子化。在系统规定中，以O_WRONLY（即阻塞方式）打开的FIFO中， 如果写入的数据长度小于等待PIPE_BUF，那么或者写入全部字节，或者一个字节都不写入。如果所有的写请求都是发往一个阻塞的FIFO的，并且每个写入请求的数据长度小于等于PIPE_BUF字节，系统就可以确保数据决不会交错在一起。<br>

# 六、FIFO与匿名管道对比
使用匿名管道，通信进程间需要为父子关系，必须由一个共同祖先启动。匿名管道的好处在于没有上面提到的数据块交错问题。

FIFO中的两个进程间没有什么必然联系，他们只是通过了一个FIFO文件来实现通信，不需要共同的祖先启动。但为了防止数据交错问题，我们采用阻塞FIFO，让写操作变成原子操作。<br>

