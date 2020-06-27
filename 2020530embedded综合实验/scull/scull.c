#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h> 		/* everything... */
#include <linux/cdev.h>
#include <asm/uaccess.h> 	/* copy_*_user */

MODULE_AUTHOR("0B703");
MODULE_LICENSE("GPL");
int scull_major = 0;
int scull_minor = 0;
struct cdev cdev; /* Char device structure */

#define MAX_SIZE 10
size_t size = 0;
char store[MAX_SIZE];


// Open and close
int scull_open(struct inode *inode , struct file *filp)
{
  // trim to 0 the length of the device if open was write -only
  
  if ( (filp -> f_flags & O_ACCMODE) == O_WRONLY) {
	size = 0;
  }
  return 0; /* success */
}

int scull_release(struct inode *inode , struct file *filp)
{
  return 0;
}


// Data management: read and write
ssize_t scull_read(struct file *filp , char __user *buf, size_t count, loff_t *f_pos)
{
  ssize_t retval = 0;
  if (*f_pos >= size)
    goto out;
  if (*f_pos + count > size)
    count = size - *f_pos;
  if (raw_copy_to_user(buf, store + *f_pos , count)) {
    retval = -EFAULT;	// 返回一个错误代码
    goto out;
  }
  *f_pos += count;
 retval = count;
out:
  return retval;
}

ssize_t scull_write(struct file *filp , const char __user *buf,size_t count ,loff_t *f_pos)
{
  ssize_t retval = -ENOMEM; /* value used in "goto out"
statements */
  if (*f_pos >= MAX_SIZE)
    goto out;
  if (*f_pos + count > MAX_SIZE)
    count = MAX_SIZE - *f_pos;
  if (raw_copy_from_user(store + *f_pos , buf, count)) {
    retval = -EFAULT;
    goto out;
  }
  *f_pos += count;
  retval = count;
  /* update the size */
  if (size < *f_pos)
    size = *f_pos;
out:
  return retval;
}

struct file_operations scull_fops = {
  .owner = THIS_MODULE ,
  .read = scull_read ,
  .write = scull_write ,
  .open = scull_open ,
  .release = scull_release ,
};
int scull_init_module(void)
{
  int result;
  dev_t dev = 0;//在linux中是unsigned int 类型，32位，用于在驱动程序中定义设备编号，高12位为主设备号，低20位为次设备号
  /*
  * Get a range of minor numbers to work with , asking for a
  dynamic major
  */
  	//alloc自动分配设备号，设备名称"scull",设备个数1，次设备号scull_minor,申请到的设备号存储在dev中。该函数返回值小于0表示申请失败。
	// dev 
  result = alloc_chrdev_region(&dev, scull_minor , 1, "scull");
  scull_major = MAJOR(dev);
  if (result < 0) {
    printk(KERN_WARNING "scull:?can't?get?major?%d\n",
           scull_major);
    return result;
  }
  /* register our device */
  cdev_init(&cdev , &scull_fops);//用上面声明的scull_fops初始化cdev。
  cdev.owner = THIS_MODULE;
  cdev.ops = &scull_fops;
  result = cdev_add (&cdev , dev, 1);//这个是在字符设备中添加一个设备。
  if (result) {
    printk(KERN_WARNING "Error?%d?adding?scull", result)
    ;
    unregister_chrdev_region(dev, 1);
    return result;
  }
  return 0; /* succeed */
}
void scull_cleanup_module(void)
{
  /* cleanup_module is never called if registering failed */
  dev_t dev;
  cdev_del(&cdev);
  dev = MKDEV(scull_major , scull_minor);
  unregister_chrdev_region(dev, 1);
}

module_init(scull_init_module);
module_exit(scull_cleanup_module);
