export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-
CC:=$(CROSS_COMPILE)gcc

obj-m := hello.o
KERNELDIR :=../../linux 
PWD := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf .cache.mk *.ko *.o modules.order Module.symvers *.mod.c *.mod .*.cmd .tmp_versions

.PHONY: modules clean
