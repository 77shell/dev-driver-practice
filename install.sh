#!/bin/bash

#
# 2016-4-24
# max.yang@delta.com.tw
#
# > Build device driver : cdata
# > Make device node : /dev/cdata
# > Remove previous kernel module : cdata.ko
# > Install kernel module : cdata.ko
#

if ! make
then
    echo -e "\033[32mMake cdata failed~\033[0m"
    exit 1
else
    echo -e "\033[33mMake cdata Okay~~\033[0m"
fi

if ! make test
then
    echo -e "\033[32mMake test failed~\033[0m"
    exit 1
fi

#
# Making device node : /dev/cdata
# Change node permission to 666
#
if ! [ -e /dev/cdata ]
then
    DEVICE_NODE=/dev/cdata
    sudo mknod $DEVICE_NODE c 121 0
    echo -e "\033[32mMaking device node\033[0m"

    if ! [ -e $DEVICE_NODE ]; then
	echo -e "\033[31mMake cdata failed~\033[0m"
	exit 1
    fi
    sudo chmod 666 $DEVICE_NODE
    ls -l 
fi

sudo rmmod cdata

if ! sudo insmod cdata.ko; then
    echo -e "\033[32mInstall driver: cdata failed~\033[0m"
    exit 1
fi

