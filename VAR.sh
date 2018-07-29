#!/bin//bash


declare -a DEVICE_NODES=(
#    /dev/cdata-misc
#    /dev/cdata-fb
#    /dev/oled-ssd1308
)

declare -a SYS_NODES=(
#    /sys/devices/platform/cdata.0
#    /sys/devices/platform/cdata-fb.0
#    /sys/devices/platform/oled.0
)

declare -a KO=(
#    cdata_plat_dev.ko
    cdata.ko
#    cdata_fb_plat_dev.ko
#    cdata_fb_ssd1308.ko
#    oled_ssd1308_spi.ko
#    oled_plat_dev.ko
)

mesg_brown()
{
    echo -e "\033[33m$1\033[0m"
}

mesg_red()
{
    echo -e "\033[31m$1\033[0m"
}

mesg_green()
{
    echo -e "\033[32m$1\033[0m"
}

error_msg()
{
    echo -e "\033[31mErrooooor:  $1\033[0m"
}
