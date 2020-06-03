```c
/* 
 * 从2.4.10版本内核开始，模块必须通过MODULE_LICENSE宏声明此模块的许可证，
 * 否则在加载此模块时，会收到内核被污染 “kernel tainted” 的警告。
 * 从linux/module.h文件中可以看到，被内核接受的有意义的许可证有
 * “GPL”，“GPL v2”，“GPL and additional rights”，“Dual BSD/GPL”，“Dual MPL/GPL”，“Proprietary”。
 */

MODULE_LICENSE("GPL")	// 模块的许可证明

// Major Numbers主设备号：代表某一类设备，标定一个确定的驱动程序。
// Minor Numbers次设备号：不同的位置，不同的操作等。标定一个具体的设备。

// 内核用inode表示文件，其中两个字段与编写驱动有关
// dev_t i_rdev，当inode指向一个字符设备文件时，存放设备编号
// struct cdev *i_cdev，字符设备的内部结构，指向struct cdev

/* linux scull函数open方法
 *
 * open 方法提供给驱动来做任何的初始化来准备后续的操作. 在大部分驱动中, open 应当 进行下面的工作:
 * 1. 检查设备特定的错误例如设备没准备好, 或者类似的硬件错误
 * 2. 如果它第一次打开, 初始化设备
 * 3. 如果需要, 更新 f_op 指针
 * 4. 分配并填充要放进 filp->private_data 的任何数据结构
 * 
 * 但是，事情的第一步常常是确定打开哪个设备. 记住 open 方法的原型是:
 */

int (*open)(struct inode *inode, struct file *filp);

/* 
 * inode参数有我们需要的信息，其中i_cdev包含我们之前建立的cdev结构，
 * 通常不使用cdev结构本身，而是使用包含cdev结构的scull_dev结构，
 * C语言中通过<linux/kernel.h>中的container_of宏实现这种转换。
 */

container_of(pointer, container_type, container_filed);

/*
 * 这个宏使用一个指向container_filed类型成员的指针，
 * 它在一个container_type类型的结构中，并且返回一个指针，该指针指向包含结构。
 *
 * 在scull_open中，这个宏用来找到适当的设备结构
 */
struct scull_dev *dev；	// device info
dev = container_of(inode->i_cdev, struct scull_dev, cdev);
filp->private_data=dev;	// for other methods

/* 
 * 一旦它找到scull_dev结构, scull在文件结构的private_data成员中存储一个它的指针, 方便以后存取。
 *
 * 识别打开的设备的另外的方法是查看存储在inode结构的次编号。
 * 如果使用register_chrdev注册设备，必须使用这个技术。
 */
```

scull_open代码：（简化过）
```c
int scull_open(struct inode *inode, struct file *filp){
	struct scull_dev *dev; /* device information */

	dev = container_of(inode->i_cdev, struct scull_dev, cdev); 
	filp->private_data = dev; /* for other methods */

 

	/* now trim to 0 the length of the device if open was write-only */ 
	if ( (filp->f_flags & O_ACCMODE) == O_WRONLY){
	scull_trim(dev); /* ignore errors */
	}

	return 0; /* success */
}

/*
 * 代码看来相当稀疏，因为在调用open时它没有做任何特别的设备处理。
 * 它不需要, 因为scull设备设计为全局的和永久的。
 * 
 * 特别地, 没有如"在第一次打开时初始化设备"等动，
 * 因为我们不为scull保持打开计数。
 */
```
