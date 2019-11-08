MODULE_NAME := hackmod
EXTRA_CFLAGS += -I$(src)

ifeq ($(KERNELRELEASE),)
# out of kernel

MODULE_SOURCES := fopskit.c newmod.c kernbitflip.c

all: $(MODULE_NAME).ko

$(MODULE_NAME).ko: $(MODULE_SOURCES)
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

test:
	dmesg --clear
	insmod hackmod.ko
	sync
	rmmod hackmod.ko
	dmesg | less

do:
	dmesg --clear
	insmod hackmod.ko
	sync

.PHONY: all clean test

else
# in-kernel / kbuild shit
ccflags-y :=  -I$(src)

obj-m := $(MODULE_NAME).o
$(MODULE_NAME)-y := fopskit.o newmod.o kernbitflip.o

endif
