#!/bin/bash

if ! make; then
    echo -e "\033[32mMake cdata failed~\033[0m"
    exit 1
fi

if ! make test; then
    echo -e "\033[32mMake test failed~\033[0m"
    exit 1
fi

if ! [ -e /dev/cdata ]; then
    mknod /dev/cdata c 121 0
    echo -e "\033[32mMaking device node\033[0m"
fi

rmmod cdata

if ! insmod cdata.ko; then
    echo -e "\033[32mInstall driver: cdata failed~\033[0m"
    exit 1
fi

#./test
#dmesg | tail -n 18
