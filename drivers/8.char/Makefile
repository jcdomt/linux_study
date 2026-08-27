obj-m := char.o

KDIR := /root/LubanCat_SDK/kernel-6.1
PWD := $(shell pwd)

ARCH := arm64
CROSS_COMPILE := /root/LubanCat_SDK/prebuilts/gcc/linux-x86/aarch64/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-

all:
	$(MAKE) -C $(KDIR) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		M=$(PWD) clean