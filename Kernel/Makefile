obj-m := helloKo.o

PWD := $(shell pwd)
KER_DIR = /lib/modules/$(shell uname -r)/build

all:
	make -C $(KER_DIR) M=$(PWD) modules

clean:
	make -C $(KER_DIR) M=$(PWD) clean
