#!/bin/bash

#
# 2016-7-07
# max.yang@delta.com.tw
#
# Install two kernel modules
#          > cdata.ko
#          > cdata_plat_dev.ko
#

error_msg()
{
    echo -e "\033[31mErrooooor:  $1\033[0m"
}

DEVICE_NODE=/dev/cdata-misc


declare -a KO_FILE=(
    cdata_plat_dev.ko
    cdata.ko
)


for i in ${KO_FILE[@]}
do
    ! insmod $i &&
	error_msg $i
done


! [ -e $DEVICE_NODE ] && \
    error_msg "No $DEVICE_NODE exists~" && \
    exit 1

lsmod | grep cdata*
dmesg | tail -n 40
ls -l $DEVICE_NODE --color
