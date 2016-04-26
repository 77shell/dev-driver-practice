#!/bin/bash

if ! make
then
    echo -e "\033[32mMake cdata failed~\033[0m"
    exit 1
else
    echo -e "\033[33mMake cdata Okay~~"
    ls -l *.ko
    echo -e "\033[0m"
fi

if ! make test
then
    echo -e "\033[32mMake test failed~\033[0m"
    exit 1
fi

my_mknod()
{
    if ! [ -e /dev/cdata ]
    then
	mknod /dev/cdata c 121 0
	echo -e "\033[32mMaking device node\033[0m"

	if ! [ -e /dev/cdata ]; then
	    echo -e "\033[31mMake cdata failed~\033[0m"
	fi
    fi
}


dmesg -C

rmmod cdata
rmmod cdata_plat_dev


if ! insmod cdata_plat_dev.ko; then
    echo -e "\033[32mInstall driver: cdata_plat_dev failed~\033[0m"
fi

if ! insmod cdata.ko; then
    echo -e "\033[32mInstall driver: cdata failed~\033[0m"
    exit 1
fi

dmesg | tail -n 40

lsmod | grep cdata
tree -hfC -H . --du -o dev.html /dev

#./test
#dmesg | tail -n 18
