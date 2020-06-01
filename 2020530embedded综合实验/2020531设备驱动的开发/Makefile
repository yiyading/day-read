# 需要创建Makefile并和模块程序放在同一目录中，运行时内部会执行两次
# 使用Kbuild规则对模块进行编译，在内核树中加入新的模块
# 参数KERNELRELEASE是在内核源码的顶层Makefile中定义的

ifneq ($(KERNELRELEASE),) 	#第二次从这里运行

# modemo-objs := file1.o file2.o file3.0 	# 如果有多个文件
	obj-m := modemo.o	# 生成modemo.ko

else	# 第一次从这里运行
	KERNELDIR = /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)	# 模块程序所在目录，在该目录运行make
all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -f *.o	# 根据需要加入操作
endif
