#include <linux/module.h>
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
