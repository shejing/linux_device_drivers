ifneq ($(KERNELRELEASE),)
	obj-m := hello.o
else
#KERNELDIR ?= ../../linux-2.6
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules 

endif

clean:
	rm -rf *.a *.mod *.order *.o  *.ko *.symvers *.mod.c
