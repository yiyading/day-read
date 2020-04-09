/* purpose: 实现一个内核模块，该模块创建/proc/tasklist文件，并且提取系统中所有进程的pid、state、和名称（comm）进行显示
 *
 */
#include<linux/module.h>
#include<linux/proc_fs.h>
#include<linux/sched/task.h>
#include<linux/seq_file.h>
#include<linux/slab.h>
#include<linux/sched/signal.h>

char modname[] = "tasklise";
struct task_struct *task;
int taskcounts = 0;		// 全局进程变量

static void *my_seq_start(struct seq_file *m, loff_t *pos){
	// 输出调试信息
	printk(KERN_INFO "Invoke start\n");	

	// 开始遍历	
	if(*pos == 0){
		task = &init_task;	// 遍历开始的记录地址
		return &task;		// 返回一个非零值表示开始遍历
	}
	// 遍历过程中
	else{	
		if(task == &init_task)
			return NULL;
		
		return (void *)pos;
	}
}

// 获取进程相关信息
static int my_seq_show(struct seq_file *m, void *v){
	printk(KERN_INFO "Invoke show\n");

	seq_printf(m, "#%-3d\t", taskcounts);	// 输出进程号
	seq_printf(m, "%d\t", task->pid);	// 输出进程pid
	seq_printf(m, "%lu\t", task->state);	// 输出进程state
	seq_printf(m, "%s\t", task->comm);	// 输出进程名称（comm）
	seq_puts(m, "\n");

	return 0;
}

static void *my_seq_next(struct seq_file *m, void *v, loff_t *pos){
	printk(KERN_INFO "Invoke next\n");
	(*pos)++;
	taskcounts++;
	task = next_task(task);	 // 指向下一个进程
	return NULL;
}

static void my_seq_stop(struct seq_file *m, void *v){
	printk(KERN_INFO "Invoke stop\n");
}

// 序列文件记录操作函数集合
static struct seq_operations my_seq_fops = {
	.start = my_seq_start,
	.next  = my_seq_next,
	.stop  = my_seq_stop,
	.show  = my_seq_show,
};

static int my_open(struct inode *inode, struct file *file){
	// 打开序列文件并关联my_seq_fops
	return seq_open(file, &my_seq_fops);
}

// proc文件操作函数集合
static const struct file_operations my_proc = {
	.owner   = THIS_MODULE,
	.open    = my_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release,
};

int __init my_init(void){
	struct proc_dir_entry *my_proc_entry;
	printk("<1>\nInstalling \'%s\' module\n", modname);
	my_proc_entry = proc_create(modname, 0x644, NULL, &my_proc); // 生成proc文件
	if(NULL == my_proc_entry)
		return -ENOMEM;

	return 0;	// SUCCESS
}

void __exit my_exit(void){
	remove_proc_entry(modname, NULL);	// 删除proc文件
	printk("<1>Removing \'%s\' modulu\n", modname);
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
