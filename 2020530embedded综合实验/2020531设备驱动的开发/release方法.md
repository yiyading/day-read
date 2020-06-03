release 方法的角色是 open 的反面。

有时你会发现方法的实现称为 device_close，而不是 device_release。

任一方式, 设备方法应当进行下面的任务:
1. 释放 open 分配在 filp->private_data 中的任何东西。
2. 在最后的 close 关闭设备。

```c
int scull_release(struct inode *inode, struct file *filp){

	printk(KERN_NOTICE "Scull released.\n");
	return 0;
}
```

你可能想知道当一个设备文件关闭次数超过它被打开的次数会发生什么。

答案简单: 不是每个 close 系统调用引起调用 release 方法. 只有真正释放设备数据结构的调用会调用这个方法。

内核维持一个文件结构被使用多少次的计数。

fork 和 dup 都不创建新文件(只有 open 这样); 它们只递增正存在的结构中的计数。

close 系统调用仅在文件结构计数掉到 0 时执行 release 方法, 这在结构被销毁时发生。

**release 方法和 close 系统调用之间的这种关系保证了你的驱动一次 open 只看到一次 release**。
