ifneq ($(KERNELRELEASE),)
	obj-m := hello.o
else
	KERNELRELEASE ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELRELEASE) M=$(PWD) modules
.PHONY: clean
clean:
	rm -rf .cache.mk *.ko *.o modules.order Module.symvers *.mod.c .*.cmd .tmp_versions
endif
