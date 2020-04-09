# PROC虚拟文件系统
/proc 文件系统是一个虚拟文件系统（没有任何一部分与磁盘相关，只存在内存中，不占用外存空间），包含了一些目录和虚拟文件。通过它可以在Linux内核空间和用户空间之间进行通信：<br>
> 向用户呈现内核中的一些信息（用cat、more等命令查看/proc文件中的信息）<br>
> 用作一种从用户空间向内核发送信息的手段（echo）<br>

Linux遵循现代操作的普遍原则：使程序员和内核、硬件等系统资源隔离开，普通用户无法看到内核空间中发生了什么，系统调用是操作系统提供给应用程序使用操作系统服务的重要接口，但同时也屏蔽了用户直接访问操作系统内核的可能性。Linux提供了LKM机制可以使我们在内核空间工作。Linux提供的LKM机制中一个重要的组成部分就是proc伪文件系统。**proc包含了当前计算机中重要的资源信息及系统信息**。<br>

LKM是用来展示/proc 文件系统的一种简单方法，因为它可以动态地向Linux 内核添加或删除代码。<br>
> LKM：动态可加载内核模块（Loadable Kernel Module）<br>

# seq_file结构体
seq_file只是在普通的文件read中加入了内核缓冲的功能，从而实现顺序多次遍历，读取大数据量的简单接口。seq_file一般只提供**只读接口**，在使用seq_file操作时，主要靠四个操作来完成内核自定义缓冲区的遍历输出操作，其中pos作为遍历的iterator，在seq_read函数中被多次使用，用以定位当前从内核自定义链表中读取的当前位置，当多次读取时，pos非常重要，且pos总是遵循从0,1,2...end+1遍历的次序，即必须作为遍历内核自定义链表的下标，也可以作为返回内容的标识。<br>

seq_file和seq_operations结构体定义在include/linux/seq_file.h：<br>
```c
struct seq_file {
	// size_t的取值范围是目标平台下最大可能的数组尺寸
	char *buf;	// 在seq_open中分配，大小为4KB
	size_t size;	// 4096
	size_t from;	// struct file从seq_file中向用户态缓冲区拷贝时相对于buf的偏移地址
	size_t count;	// 可以拷贝到用户态的字符数目
	size_t pad_until;	//
	loff_t index;	// 从内核态向seq_file的内核态缓冲区buf中拷贝时,
			// start、next的处理的下标pos数值，即用户自定义遍历iter
	loff_t read_pos;	// 当前已拷贝到用户态的数据量大小，
				// 即struct file中拷贝到用户态的数据量
	u64 version;
	struct mutex lock;	// 保护该seq_file的互斥锁结构
	const struct seq_operations *op;	// seq_start,seq_next,set_show,seq_stop函数结构体
	int poll_event;

#ifdef CONFIG_USER_NS
	struct user_namespace *user_ns;
#endif

	void *private;
};
 
struct seq_operations {
	void * (*start) (struct seq_file *m, loff_t *pos);
	void (*stop) (struct seq_file *m, void *v);
	void * (*next) (struct seq_file *m, void *v, loff_t *pos);
	int (*show) (struct seq_file *m, void *v);
};
```

要使用seq_file接口，首先需要定义一个file_operations结构体，以实现open方法：
```c
static const struct file_operations debug_dump_info_fops = {
	.owner		= THIS_MODULE,
	.open		= debugfs_dump_info_open,
	.read		= seq_read,
	.release	= single_release,
};
```

debugfs_dump_info_open定义：
```c
static int debugfs_dump_info_open(struct inode *inode, struct file *file{
	return single_open(file, msg21xx_debug_dump_info, inode->i_private);
}
```
* 原子操作：不会被线程调度机制打断的操作；这种操作一旦开始，就一直运行到结束。<br>
inode-\>i\_private中保存了驱动的私有数据结构地址，msg21xx\_debug\_dump\_info函数就是seq\_file的show方法，内核在设计上，seq\_file的代码不会在start和stop调用之间执行其他的非原子操作，内核在调用start之后，马上就会调用stop，所以在start方法中获取信号量或者自旋锁是安全的，**在调用start和stop之间，内核会调用show方法即msg21xx_debug_dump_info将实际的数据输出到用户空间**，msg21xx_debug_dump_info调用seq_printf函数将data->ts_info指向的数据输出到m所指向的buf中，seq_printf会调用seq_vprintf，seq_vprintf调用vsnprintf将数据输出到buf中。<br>
```c
static int msg21xx_debug_dump_info(struct seq_file *m, void *v){
	struct msg21xx_ts_data *data = m->private;
	seq_printf(m, "%s\n", data->ts_info);
	return 0;
}
```

seq_printf和seq_vprintf均定义在fs/seq_file.c文件中：
```c
int seq_vprintf(struct seq_file *m, const char *f, va_list args) {
	int len;
 
	if (m->count < m->size) {
		len = vsnprintf(m->buf + m->count, m->size - m->count, f, args);
		if (m->count + len < m->size) {
			m->count += len;
			return 0;
		}
	}
	seq_set_overflow(m);
	return -1;
}

EXPORT_SYMBOL(seq_vprintf);
 
int seq_printf(struct seq_file *m, const char *f, ...) {
	int ret;
	va_list args;
 
	va_start(args, f);
	ret = seq_vprintf(m, f, args);
	va_end(args);
 
	return ret;
}
EXPORT_SYMBOL(seq_printf);
```

debugfs_dump_info_open函数调用single_open来初始化seq_operations中的start，stop，next，show函数指针。
```c
int single_open(struct file *file, int (*show)(struct seq_file *, void *), void *data) {
	struct seq_operations *op = kmalloc(sizeof(*op), GFP_KERNEL);
	int res = -ENOMEM;
 
	if (op) {
		op->start = single_start;
		op->next = single_next;
		op->stop = single_stop;
		op->show = show;
		res = seq_open(file, op);
		if (!res)
			((struct seq_file *)file->private_data)->private = data;
		else
			kfree(op);
	}
	return res;
}
```

seq_open中分配seq_file结构体，通过file->private_data = p;将file的private_data指针指向分配的seq_file结构，再通过single_open函数中的((struct seq_file *)file->private_data)->private = data;将驱动的私有数据结构的地址赋值给seq_file结构的private指针，这样一来，在msg21xx_debug_dump_info函数中，就可以通过struct msg21xx_ts_data *data = m->private;获得驱动的数据结构地址。<br>

```c
/**
 *	seq_open -	initialize sequential file
 *	@file: file we initialize
 *	@op: method table describing the sequence
 *
 *	seq_open() sets @file, associating it with a sequence described
 *	by @op.  @op->start() sets the iterator up and returns the first
 *	element of sequence. @op->stop() shuts it down.  @op->next()
 *	returns the next element of sequence.  @op->show() prints element
 *	into the buffer.  In case of error ->start() and ->next() return
 *	ERR_PTR(error).  In the end of sequence they return %NULL. ->show()
 *	returns 0 in case of success and negative number in case of error.
 *	Returning SEQ_SKIP means "discard this element and move on".
 */
int seq_open(struct file *file, const struct seq_operations *op) {
	struct seq_file *p = file->private_data;
 
	if (!p) {
		p = kmalloc(sizeof(*p), GFP_KERNEL);
		if (!p)
			return -ENOMEM;
		file->private_data = p;
	}

	memset(p, 0, sizeof(*p));
	mutex_init(&p->lock);
	p->op = op;

#ifdef CONFIG_USER_NS
	p->user_ns = file->f_cred->user_ns;
#endif
 
	/*
	 * Wrappers around seq_open(e.g. swaps_open) need to be
	 * aware of this. If they set f_version themselves, they
	 * should call seq_open first and then set f_version.
	 */
	file->f_version = 0;
 
	/*
	 * seq_files support lseek() and pread().  They do not implement
	 * write() at all, but we clear FMODE_PWRITE here for historical
	 * reasons.
	 *
	 * If a client of seq_files a) implements file.write() and b) wishes to
	 * support pwrite() then that client will need to implement its own
	 * file.open() which calls seq_open() and then sets FMODE_PWRITE.
	 */

	file->f_mode &= ~FMODE_PWRITE;
	return 0;
}
```

最后只需调用debugfs_create_file完成所有的工作，并将驱动的私有数据结构指针传递到struct inode结构的inode->i_private。<br>
```c
temp = debugfs_create_file("dump_info",
			   S_IRUSR | S_IWUSR,
			   data->dir,
			   data, &debug_dump_info_fops);
	if (temp == NULL || IS_ERR(temp)) {
		pr_err("debugfs_create_file failed: rc=%ld\n", PTR_ERR(temp));
		err = PTR_ERR(temp);
		goto free_debug_dir;
	}
```
