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


declare -a KO=(
    cdata_plat_dev.ko
    cdata.ko
)

DEVICE_NODE=/dev/cdata-misc

install_modules()
{
    sudo dmesg -C
    
    for i in ${KO[@]}
    do
	! sudo insmod $i \
	    && echo "Install driver: $i failed~"
    done
    
    if [ ! -e $DEVICE_NODE ]; then
	echo -e "\033[31mNo device node, /dev/cdata-misc\033[0m"
    else
	! sudo chmod 666 $DEVICE_NODE && echo "Change permission of $DEVICE_NODE failed~"
    fi

    lsmod | grep cdata
    echo -e "\033[33mInstall modules complete."
    dmesg | tail -n 40
    tree -hfC -H . --du -o dev.html /dev
    ls -l /dev/cdata-misc
    echo -e "\033[0m"
}


uninstall_modules()
{
    for i in ${KO[@]}
    do
	! sudo rmmod $i \
	    && echo "Uninstall driver: $i failed~"
    done
    echo -e "\033[33mUninstall modules\033[0m"
}


clean_modules()
{
    for i in ${KO[@]}
    do
	! rm $i
    done
    echo -e "\033[33mDelete modules\033[0m"
}


case $1 in
    insmod)
        install_modules
	exit 0
	;;

    rmmod)
	uninstall_modules
	exit 0
	;;

    clean)
	clean_modules
	;;

    --help)
	echo -e "\033[33m\n\t./install.sh [ ins | rm ]\n\033[0m"
	exit 0
	;;
esac



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


[ $# -eq 0 ] && echo -e "\033[33m\t./build.sh [ insmod | rmmod | clean ]\033[0m\n"


#
# Making device node : /dev/cdata
# Change node permission to 666
#

my_mknod()
{
    if ! [ -e /dev/cdata ]
    then

	sudo mknod $DEVICE_NODE c 121 0
	echo -e "\033[32mMaking device node\033[0m"

	if ! [ -e $DEVICE_NODE ]; then
	    echo -e "\033[31mMake cdata failed~\033[0m"
	    exit 1
	fi
	sudo chmod 666 $DEVICE_NODE
	ls -l 
    fi
}
