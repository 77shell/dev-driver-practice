#!/bin/bash

#
# 2016-7-07
# max.yang@delta.com.tw
#
# Uninstall two kernel modules
#          > cdata.ko
#          > cdata_plat_dev.ko
#

error_msg()
{
    echo -e "\033[31mErrooooor:  $1\033[0m"
}


declare -a KO=(
    cdata_plat_dev.ko
    cdata.ko
)

DEVICE_NODE=/dev/cdata-misc


for i in ${KO[@]}
do
    ! rmmod $i && \
	error_msg $i
done

echo -e "\033[33mUninstall modules\033[0m"
