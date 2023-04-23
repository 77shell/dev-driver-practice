# 
# 2016-April-23
# Max <max.yang@deltaww.com>
#
# To build kernel module
#

KDIR := /lib/modules/$(shell uname -r)/build
CFLAGS_user = -std=gnu99 -Wall -Wextra -Werror

obj-m := oled_plat_dev.o \
	oled_ssd1308.o

oled_ssd1308-objs := oled_ssd1308_spi.o \
		oled_ssd1308_ctrl.o \
		oled_ssd1308_gpio.o

# ccflags-y := -include$(PWD)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

.PHONY: clean
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

.PHONY: TAGS
TAGS:
	rm -rf TAGS
	find . -regextype posix-egrep -iregex '.*\.(cpp|c|h)' | xargs etags -a
	find . -iname Makefile.* | xargs etags -a

