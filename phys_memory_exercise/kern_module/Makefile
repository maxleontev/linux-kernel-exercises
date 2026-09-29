obj-m += test_module.o
test_module-y := main_m.o solution.o

KERN_DIR := /lib/modules/$(shell uname -r)/build

all:
	make -C $(KERN_DIR) M=$(PWD) modules

clean:
	make -C $(KERN_DIR) M=$(PWD) clean

