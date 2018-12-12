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

CFLAGS += -pthread -iquote../../
CPPFLAGS += -pthread -iquote../../

vpath %.c ../
vpath %.cpp ../

BINS := test \
	test-fb-ssd1308 \
	test-oled-ssd1308 \
	test-oled-ssd1308-r1 \
	rolling-oled-ssd1308

.PHONY: all
all: $(BINS)

test-oled-ssd1308-r1: test-oled-ssd1308-r1.o input-event.o
	$(CXX) $(CPPFLAGS) $^ -o $@

clean:
	rm -rf $(BINS) .#* *~


.PHONY: TAGS
TAGS:
	rm -rf TAGS
	find ../ -regextype posix-egrep -iregex '.*\.(cpp|c|h)' | xargs etags -a
	find ~/triton/toolchain/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabihf/arm-linux-gnueabihf/libc/usr/include \
		-type f | xargs etags -a
	find ~/triton/toolchain/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabihf/arm-linux-gnueabihf/include/c++/5.3.1 \
		-type f | xargs etags -a 
	find . -iname 'Makefile.*' -type f | xargs etags -a
