/* 
 * 从2.4.10版本内核开始，模块必须通过MODULE_LICENSE宏声明此模块的许可证，
 * 否则在加载此模块时，会收到内核被污染 “kernel tainted” 的警告。
 * 从linux/module.h文件中可以看到，被内核接受的有意义的许可证有
 * “GPL”，“GPL v2”，“GPL and additional rights”，“Dual BSD/GPL”，“Dual MPL/GPL”，“Proprietary”。
 */
MODULE_LICENSE("GPL")	// 模块的许可证明

Major Numbers主设备号：代表某一类设备，标定一个确定的驱动程序。

Minor Numbers次设备号：不同的位置，不同的操作等。标定一个具体的设备。


