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
	cdata_fb_plat_dev.o \
	oled_plat_dev.o \
	oled_ssd1308.o

oled_ssd1308-objs := oled_ssd1308_spi.o \
			oled_ssd1308_ctrl.o \
			oled_ssd1308_gpio.o


KDIR := /usr/src/linux-headers-$(shell uname -r)
PWD := $(shell pwd)
ccflag-y += "-iquote./"

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.o *.ko .*cmd modules.* Module.* .tmp_versions *.mod.c .#* *~
