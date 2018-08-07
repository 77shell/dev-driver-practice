
CORTEX_FLAGS := -march=armv7-a -marm -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a8

KDIR 	     := ~/triton/board-support/ti-linux-kernel-4.1.13-rt
SDK          := ${HOME}/triton/toolchain/gcc-linaro-4.9-2015.05-x86_64_arm-linux-gnueabihf/arm-linux-gnueabihf

CFLAGS 	     := $(CORTEX_FLAGS)
LIBSPATH     := 
INCLUDEPATH  := -I${SDK}/include

CROSS_COMPILE := arm-linux-gnueabihf-
CXX	      := $(CROSS_COMPILE)g++
CC	      := $(CROSS_COMPILE)gcc
AR	      := $(CROSS_COMPILE)ar

KBUILD_VERBOSE:= 0
export
