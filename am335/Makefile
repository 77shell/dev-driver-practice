#
# 
# 2016-July-06
# Max <max.yang@deltaww.com>
#
# To build kernel module
# 	> cdata.ko
#	> cdata_plat_dev.ko
#

include ./Makefile.inc


vpath %.c ../

obj-m 	:= cdata.o \
	cdata_plat_dev.o \
	cdata_fb_ssd1308.o \
	cdata_fb_plat_dev.o \
	oled_plat_dev.o \
	oled_ssd1308.o

oled_ssd1308-objs := oled_ssd1308_spi.o oled_ssd1308_ctrl.o
PWD 	:= $(shell pwd)


.PHONY: all
all:
	@echo "\033[33mPWD : $(PWD)\033[0m"
	@echo "\033[33mCFLAG : $(CFLAGS)\033[0m"
	@echo "\033[33mCC : $(CC)\033[0m"
	$(MAKE) ARCH=arm -C $(KDIR) SUBDIRS=$(PWD)/.. modules

clean:
	rm -rf ../*.o ../*.ko ../.*cmd ../modules.* ../Module.* ../.tmp_versions ../*.mod.c .#* *~ .tmp_versions


.PHONY: TAGS
TAGS:
	rm -rf TAGS
	find ../ -regextype posix-egrep -iregex '.*\.(cpp|c|h)' | xargs etags -a
	find ~/gcc-linaro-arm-linux-gnueabihf-4.7-2013.03-20130313_linux/arm-linux-gnueabihf/libc/usr/include \
		-type f | xargs etags -a
	find ~/gcc-linaro-arm-linux-gnueabihf-4.7-2013.03-20130313_linux/arm-linux-gnueabihf/include/c++/4.7.3 \
		-type f | xargs etags -a 
	find . -iname 'Makefile.*' -type f | xargs etags -a
