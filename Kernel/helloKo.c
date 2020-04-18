#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Sean Deep");

static int __init hello_init(void)
{
	printk("Hello, yyd!\n");
	return 0;
}

static void __exit hello_exit(void)
{
	printk("Exit, yyd!\n");
}

module_init(hello_init);
module_exit(hello_exit);
