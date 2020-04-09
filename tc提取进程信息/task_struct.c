



int route_struct_init(void){
	struct task_struct *task = current;
	int i;

	printk(KERN_NOTICE "entering module");
	printk(KERN_NOTICE "0: This task is callen %d, his pid is %d\n", task->comm, task->pid);

	// 创建proc文件
	my_proc_creat = proc_create(modnaame, 0x644, NULL, &my_proc);	// 生成proc文件

	for(i = 1; takd->pid != 0; ++i){
		// 让task指向他的父进程
		// 打印这个进程的comm、pid信息
	}

	return 0;
}
