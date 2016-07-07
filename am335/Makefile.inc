
CORTEX_FLAGS := -march=armv7-a -marm -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a8

KDIR 	     := ~/triton/board-support/ti-linux-kernel-4.1.13-rt
SDK          := ${HOME}/ti-processor-sdk-linux-am335x-evm-02.00.01.07/linux-devkit

CFLAGS 	     := $(CORTEX_FLAGS)
LIBSPATH     := 
INCLUDEPATH  := -I${SDK}/sysroots/cortexa8hf-vfp-neon-linux-gnueabi/usr/include

CROSS_COMPILE := arm-linux-gnueabihf-
CXX	      := $(CROSS_COMPILE)g++
CC	      := $(CROSS_COMPILE)gcc
AR	      := $(CROSS_COMPILE)ar

KBUILD_VERBOSE:= 0
export
