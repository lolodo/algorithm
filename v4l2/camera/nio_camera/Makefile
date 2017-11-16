ifneq ($(KERNELRELEASE),)
	obj-m := max9286_nio.o
else
	KERNELDIR := /home/fangyuan/workspace/yocto/build/tmp/work/auto2712m1v1_ivi_agl-agl-linux/linux-mtk-extension/4.9-r0/linux-mtk-extension-4.9
    CCPREFIX := /home/fangyuan/workspace/yocto/build/tmp/sysroots/x86_64-linux/usr/bin/aarch64-agl-linux/aarch64-agl-linux-
	PWD :=$(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules ARCH=arm64 CROSS_COMPILE=${CCPREFIX}
clean:
	rm -rf *.o *.mod.o *.mod.c *.symvers modul* .h* .b* .M* .t* *.ko *.cmd
endif
