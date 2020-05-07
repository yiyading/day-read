#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");
static int book_init(void)
{
        printk(KERN_EMERG "EMERG/n");
        printk(KERN_ALERT "ALERT/n");
        printk(KERN_CRIT " CRIT/n");
        printk(KERN_ERR " ERR/n");
        printk(KERN_WARNING "WARNING/n");
        printk(KERN_NOTICE "NOTICE/n");
        printk(KERN_INFO "INFO/n");
        printk(KERN_DEBUG "DEBUG/n");
        return 0;
}

static void book_exit(void)
{
        printk(KERN_ALERT "Book module exit/n");
}

module_init(book_init);
module_exit(book_exit);
