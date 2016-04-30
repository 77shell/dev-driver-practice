#!/bin/bash


declare -a KO=(
    cdata_plat_dev.ko
    cdata.ko
)


install_modules()
{
    sudo dmesg -C
    
    for i in ${KO[@]}
    do
	! sudo insmod $i \
	    && echo "Install driver: $i failed~"
    done
    
    lsmod | grep cdata
    echo -e "\033[33mInstall modules complete.\033[0m"
    dmesg | tail -n 40
    tree -hfC -H . --du -o dev.html /dev
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
    ins)
        install_modules
	exit 0
	;;

    rm)
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


my_mknod()
{
    if ! [ -e /dev/cdata ]
    then
	mknod /dev/cdata c 121 0
	echo -e "\033[32mMaking device node\033[0m"

	if ! [ -e /dev/cdata ]; then
	    echo -e "\033[31mMake cdata failed~\033[0m"
	    exit 1
	fi
    fi
    sudo chmod 666 /dev/cdata
}

if ! [ -e /dev/cdata ]
then
    sudo mknod /dev/cdata c 121 0
    echo -e "\033[32mMaking device node\033[0m"

    if ! [ -e /dev/cdata ]; then
	echo -e "\033[31mMake cdata failed~\033[0m"
	exit 1
    fi
    sudo chmod 666 /dev/cdata
fi

sudo rmmod cdata

if ! sudo insmod cdata.ko; then
    echo -e "\033[32mInstall driver: cdata failed~\033[0m"
    exit 1
fi

