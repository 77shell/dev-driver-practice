#!/bin/bash

#export SDK_DIR="/home/max/gcc-linaro-arm-linux-gnueabihf-4.7-2013.03-20130313_linux"
export SDK_DIR="${HOME}/ti-processor-sdk-linux-am335x-evm-02.00.01.07/linux-devkit/sysroots/x86_64-arago-linux/usr"
export PATH="$PATH:${SDK_DIR}/bin"

export CROSS_COMPILE=arm-linux-gnueabihf-
export CXX=${CROSS_COMPILE}g++
export CC=${CROSS_COMPILE}gcc
export AR=${CROSS_COMPILE}ar

export KBUILD_VERBOSE=0
