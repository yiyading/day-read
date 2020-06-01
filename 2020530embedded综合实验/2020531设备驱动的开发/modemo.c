#include<linux/init.h>
#include<linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");

static int hello_init(void){
	printk(KERN_ALERT "Hello, world1\n");
	printk(KERN_ALERT "HELLO, jingwen LUO2\n");
	return 0;	
}

// add more driver functions
static void hello_exit(void){
	printk(KERN_ALERT "Goodbye, cruel world3\n");
	printk(KERN_ALERT "Goodbye, cruel LUO4\n");
}

module_init(hello_init);
module_exit(hello_exit);
