#!/bin/bash

#
# 2016-7-07
# max.yang@delta.com.tw
#
# Install kernel modules:
#          > cdata.ko
#          > cdata_plat_dev.ko
#          > cdata_fb_ssd1308.ko
#          > cdata_fb_plat_dev.ko
#          > oled_ssd1308_spi.ko
#          > oled_plat_dev.ko
#

error_msg()
{
    echo -e "\033[31mErrooooor:  $1\033[0m"
}


declare -a DEVICE_NODES=(
    #/dev/cdata-misc
    #/dev/cdata-fb
    /dev/oled-ssd1308
)

declare -a KO_FILE=(
    #cdata_plat_dev.ko
    #cdata.ko
    #cdata_fb_plat_dev.ko
    #cdata_fb_ssd1308.ko
    oled_plat_dev.ko
    oled_ssd1308.ko
)


for i in ${KO_FILE[@]}
do
    ! insmod $i &&
	error_msg $i
done


for i in ${DEVICE_NODES[@]}
do
    ! [ -e $i ] && \
	error_msg "No $i exists~" && \
	exit 1
done


lsmod | grep cdata*
dmesg | tail -n 10
ls -l $DEVICE_NODE --color
