#
# 
# 2016-July-06
# Max <max.yang@deltaww.com>
#
# To build kernel module
#

include $(PWD)/Makefile.inc


obj-m 	:= cdata.o \
	cdata_plat_dev.o \
	cdata_fb_ssd1308.o \
	cdata_fb_plat_dev.o \
	oled_plat_dev.o \
	oled_ssd1308.o

cdata-y			:= ../cdata.o
cdata_plat_dev-y	:= ../cdata_plat_dev.o
cdata_fb_ssd1308-y 	:= ../cdata_fb_ssd1308.o
cdata_fb_plat_dev-y	:= ../cdata_fb_plat_dev.o
oled_plat_dev-y		:= ../oled_plat_dev.o
oled_ssd1308-y		:= ../oled_ssd1308_spi.o \
				../oled_ssd1308_ctrl.o \
				../oled_ssd1308_gpio.o \
				../oled_ssd1308_attr.o

# turn on pr_debug()
#CFLAGS_oled_ssd1308_spi.o  += -DDEBUG
#CFLAGS_oled_ssd1308_ctrl.o += -DDEBUG
#CFLAGS_oled_ssd1308_gpio.o += -DDEBUG

PWD 	:= $(shell pwd)

.PHONY: all
all:
	@echo "\033[33m\tPWD   : $(PWD)\033[0m"
	@echo "\033[33m\tCFLAG : $(CFLAGS)\033[0m"
	@echo "\033[33m\tCC    : $(CC)\033[0m"
	@echo "\033[33m\tKDIR  : $(KDIR)\033[0m"
	@echo "\033[33m\tobj-m : $(obj-m)\033[0m"
	@echo "\033[33m\toled_ssd1308-y : $(oled_ssd1308-y)\033[0m"
	$(MAKE) ARCH=arm -C $(KDIR) M=`pwd` modules

clean:
	rm -rf ../*.o *.o *.ko .*cmd modules.* Module.* .tmp_versions *.mod.c .#* *~ .tmp_versions


.PHONY: TAGS
TAGS:
	rm -rf TAGS
	find ../ -regextype posix-egrep -iregex '.*\.(cpp|c|h)' | xargs etags -a
	find  ~/triton/toolchain/gcc-linaro-4.9-2015.05-x86_64_arm-linux-gnueabihf/arm-linux-gnueabihf/include \
		-type f | xargs etags -a
	find . -iname 'Makefile.*' -type f | xargs etags -a
