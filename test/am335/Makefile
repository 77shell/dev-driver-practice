#
# 
# 2016-July-06
# Max <max.yang@deltaww.com>
#
# To build kernel module
# 	> cdata.ko
#	> cdata_plat_dev.ko
#

include ../../am335/Makefile.inc

DBG=y

ifneq ($(DBG),y)
	DEBUG=
	OPTIMIZE=-O2
else
	DEBUG=-g
	OPTIMIZE=-O0
endif

CFLAGS += -iquote../../ -lpthread

vpath %.c ../

BINS := test \
	test-fb-ssd1308 \
	test-oled-ssd1308

.PHONY: all
all: $(BINS)


clean:
	rm -rf $(BINS) .#* *~


.PHONY: TAGS
TAGS:
	rm -rf TAGS
	find ../ -regextype posix-egrep -iregex '.*\.(cpp|c|h)' | xargs etags -a
	find ~/gcc-linaro-arm-linux-gnueabihf-4.7-2013.03-20130313_linux/arm-linux-gnueabihf/libc/usr/include \
		-type f | xargs etags -a
	find ~/gcc-linaro-arm-linux-gnueabihf-4.7-2013.03-20130313_linux/arm-linux-gnueabihf/include/c++/4.7.3 \
		-type f | xargs etags -a 
	find . -iname 'Makefile.*' -type f | xargs etags -a
