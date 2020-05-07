# 一、什么是消息队列
消息队列提供了一种从一个进程向另一个进程发送一个数据块的方法。  每个数据块都被认为含有一个类型，接收进程可以独立地接收含有不同类型的数据结构。我们可以通过发送消息来避免命名管道的同步和阻塞问题。但是消息队列与命名管道一样，每个数据块都有一个最大长度的限制。<br>

Linux用宏MSGMAX和MSGMNB来限制一条消息的最大长度和一个队列的最大长度。<br>

# 二、消息队列的原理
Linux提供了一系列消息队列的函数接口来让我们方便地使用它来实现进程间的通信。它的用法与其他两个System V PIC机制，即信号量和共享内存相似。<br>

## 1.msgget()函数
该函数用来创建和访问一个消息队列。它的原型为：<br>
```c
int msgget(key_t key, int msgflg);
```
返回：成功返回信号量标识符（非零整数），失败返回-1。<br>

第一个参数：键值唯一标识一个消息队列；与其他的IPC机制一样，程序必须提供一个键来命名某个特定的消息队列。<br>

第二个参数：创建模式与访问权限标志位，创建模式有：
* 0：取信号量标识符，若不存在则函数会报错。<br>
* IPC_CREAT：如果不存在键值与key相等的信号量，则新建信号量；如果存在这样的信号量，返回此信号量的标识符。<br>
* IPC_CREAT|IPC_EXCL：如果不存在键值与key相等的信号量，则新建信号量；如果存在这样的信号量则报错。<br>

## 2.msgsend()函数
该函数用来把消息添加到消息队列中。它的原型为：<br>
```c
int msgsend(int msgid, const void *msg_ptr, size_t msg_sz, int msgflg);
```
返回：成功返回0，失败返回-1。<br>

第一个参数：msgget()返回的队列标识符。<br>

第二个参数：指向发送消息结构体的指针，但是消息的数据结构却有一定的要求，指针msg_ptr所指向的消息结构一定要是以一个长整型成员变量开始的结构体，接收函数将用这个成员来确定消息的类型。所以消息结构要定义成这样：<br>
```c
struct my_message {
    long int message_type;
    /* The data you wish to transfer */
};
```
第三个参数：msg_ptr指向的消息的长度，注意是消息的长度，而不是整个结构体的长度，也就是说msg_sz是不包括长整型消息类型成员变量的长度。即消息字节数，不含消息类型占用的4个字节。<br>

第四个参数：用于控制当前消息队列满或队列消息到达系统范围的限制时将要发生的事情。<br>
* 0：若消息队列已满，阻塞等待直到阻塞被解除。阻塞解除条件可为：1)消息队列中有容纳该消息的空间，2)消息队列被删除，3)调用msgsnd()的进程被信号中断。<br>
* IPC_NOWAIT：若消息队列已满，msgsnd()不会阻塞等待，而是返回-1。<br>
* IPC_NOERROR：若发送的消息字节数大于msgsize，则把该消息截断，截断部分将被丢弃。<br>

## 3.msgrcv()函数
该函数用来从一个消息队列获取消息，它的原型为:<br>
```c
int msgrcv(int msgid, void *msg_ptr, size_t msg_st, long int msgtype, int msgflg);
```
返回：成功返回接收到的字节数，失败返回-1。<br>

msgid, msg_ptr, msg_st的作用与函数msgsnd()中的前三个参数的相同。<br>

第四个参数：接收消息类型参数<br>
* msgtyp等于0，则返回队列的最早的一个消息.<br>
* msgtyp大于0，则返回对应类型的第一个消息.<br>
* msgtyp小于0，则返回其类型小于或等于msgtyp绝对值的第一个消息.<br>

第五个参数：用于控制当队列中没有相应类型的消息可以接收时将发生的事情.<br>
* 0：若消息队列为空，阻塞等待直到阻塞被解除。阻塞解除条件可为：1)消息队列中有了满足条件的消息，2)消息队列被删除，3)调用msgrcv()的进程被信号中断.<br>
* IPC_NOWAIT：若消息队列为空，msgrcv()不会阻塞等待，而是返回-1.<br>
* IPC_EXCEPT：与msgtype配合使用，返回队列中第一个类型不为msgtype的消息.<br>
* IPC_NOERROR：若接收的消息字节数大于msg_st，则把该消息截断，截断部分将被丢弃.<br>

## 4.msgctl()函数
该函数用来控制消息队列，它与共享内存的shmctl函数相似，它的原型为：<br>
```c
int msgctl(int msgid, int command, struct msgid_ds *buf);
```
返回：成功返回0，失败返回-1<br>

第一个参数：msgget()返回的队列标识符.<br>

第二个参数：将要采取的动作，它可以取3个值：
* 1.IPC_STAT：读取消息队列的数据结构msgid_ds，并存储在buf指定的地址中。对于每一个队列都有一个msgid_ds结构来描述队列当前的状态.<br>
* 2.IPC_SET：用buf参数设置消息队列的数据结构msgid_ds中的ipc_perm元素的值.<br>
* 3.IPC_RMID：删除消息队列.<br>

第三个参数：消息队列缓冲区；指向msgid_ds结构的指针，它指向消息队列模式和访问权限的结构。msgid_ds结构至少包括以下成员：<br>
```c
struct msgid_ds
{
    uid_t shm_perm.uid;
    uid_t shm_perm.gid;
    mode_t shm_perm.mode;
};
```

# 三、使用消息队列进行进程间通信
编写两个程序，msgreceive()和msgsned()来表示接收和发送信息。根据正常的情况，我们允许两个程序都可以创建消息，但只有接收者在接收完最后一个消息之后，它才把它删除。

源文件[msgreceive.c](https://github.com/yiyading/Embedded-software/blob/master/%E6%B6%88%E6%81%AF%E9%98%9F%E5%88%97%E9%80%9A%E4%BF%A1/msgreceive.c)

源代码[msgsend.c](https://github.com/yiyading/Embedded-software/blob/master/%E6%B6%88%E6%81%AF%E9%98%9F%E5%88%97%E9%80%9A%E4%BF%A1/msgsend.c)

# 四、例子分析——消息类型
注意msgreceive.c文件main()函数中定义的变量msgtype（注释为注意1），它作为msgrcv()函数的接收信息类型参数的值，其值为0，表示获取队列中第一个可用的消息。再来看看msgsend.c文件中while循环中的语句data.msg_type = 1（注释为注意2），它用来设置发送的信息的信息类型，即其发送的信息的类型为1。所以程序msgreceive()能够接收到程序msgsend()发送的信息。<br>

如果把注意1，即msgreceive.c文件main()函数中的语句由long int msgtype = 0;改变为long int msgtype = 2;会发生什么情况，msgreceive()将不能接收到程序msgsend()发送的信息。因为在调用msgrcv()函数时，如果msgtype（第四个参数）大于零，则将只获取具有相同消息类型的第一个消息，修改后获取的消息类型为2，而msgsend()发送的消息类型为1，所以不能被msgreceive()程序接收。<br>

# 五、消息队列和命名管道的比较
消息队列跟命名管道有不少的相同之处，通过与命名管道一样，消息队列进行通信的进程可以是不相关的进程，同时它们都是通过发送和接收的方式来传递数据的。在命名管道中，发送数据用write()，接收数据用read()，则在消息队列中，发送数据用msgsnd()，接收数据用msgrcv()。而且它们对每个数据都有一个最大长度的限制。<br>

与命名管道相比，消息队列的优势在于：<br>
> 1、消息队列也可以独立于发送和接收进程而存在，从而消除了在同步命名管道的打开和关闭时可能产生的困难。<br>
2、同时通过发送消息还可以避免命名管道的同步和阻塞问题，不需要由进程自己来提供同步方法。<br>
3、接收程序可以通过消息类型有选择地接收数据，而不是像命名管道中那样，只能默认地接收。<br>
