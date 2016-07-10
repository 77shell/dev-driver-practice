#!/bin/bash


case $1 in
    insmod)
	sudo insmod cdata_fb_plat_dev.ko
	lsmod | grep cdata
	ls -l /sys/devices/platform
	;;

    rmmod)
	sudo rmmod cdata_fb_plat_dev
	lsmod | grep cdata
	ls -l /sys/devices/platform
	;;
esac

