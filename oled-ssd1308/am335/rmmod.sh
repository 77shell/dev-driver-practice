#!/bin/bash

#
# 2016-7-07
# max.yang@delta.com.tw
#
# Uninstall kernel modules:
#          > cdata.ko
#          > cdata_plat_dev.ko
#          > cdata_fb_ssd1308.ko
#          > cdata_fb_plat_dev.ko
#

error_msg()
{
    echo -e "\033[31mErrooooor:  $1\033[0m"
}

declare -a DEVICE_NODES=(
    /dev/cdata-misc
    /dev/cdata-fb
)

declare -a KO=(
#    cdata_plat_dev.ko
#    cdata.ko
#    cdata_fb_plat_dev.ko
#    cdata_fb_ssd1308.ko
    oled_ssd1308.ko
    oled_plat_dev.ko
)

for i in ${KO[@]}
do
    ! rmmod $i && \
	error_msg $i
done

for i in ${DEVICE_NODES[@]}
do
    [ -e $i ] && \
	error_msg "$i is still there~" && \
	exit 1
done
echo -e "\033[33m"
lsmod
echo -e "\nUninstall modules\033[0m"
