# 
# 2016-April-23
# Max <max.yang@deltaww.com>
#
# To build kernel module
# 	> cdata.ko
#	> cdata_plat_dev.ko
#	> cdata_fb.ko
#	> cdata_fb_plat_dev.ko
#


obj-m := cdata.o \
	cdata_plat_dev.o \
	cdata_fb_ssd1308.o \
	cdata_fb_plat_dev.o

KDIR := /usr/src/linux-headers-$(shell uname -r)
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.o *.ko .*cmd modules.* Module.* .tmp_versions *.mod.c .#* *~
