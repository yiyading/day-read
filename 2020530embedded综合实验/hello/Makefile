ifneq ($(KERNELRELEASE),)   #第二次从这里运行

# modemo-objs := file1.o file2.o file3.0    # 如果有多个文件
	obj-m := hello.o   # 生成modemo.ko
else    # 第一次从这里运行
	KERNELDIR = /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd) # 模块程序所在目录，在该目录运行make
all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -f *.o   # 根据需要加入操作
endif

