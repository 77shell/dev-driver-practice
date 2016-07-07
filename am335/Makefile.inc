
CORTEX_FLAGS := -march=armv7-a -marm -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a8
CFLAGS 	     := $(CORTEX_FLAGS)
KDIR 	     := ~/triton/board-support/ti-linux-kernel-4.1.13-rt

SDK          := ${HOME}/ti-processor-sdk-linux-am335x-evm-02.00.01.07/linux-devkit
SRC_ROOT     := ${HOME}/src/csu.linux

LIBSPATH     := -L${SRC_ROOT}/am335/usr/local/lib
INCLUDEPATH  := -I${SRC_ROOT}/am335/usr/local/include \
	     	-I${SDK}/sysroots/cortexa8hf-vfp-neon-linux-gnueabi/usr/include