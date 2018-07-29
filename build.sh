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

. ./VAR.sh


clean_modules()
{
    for i in ${KO[@]}
    do
	! rm $i
    done

    mesg_brown "Delete modules"
}


case $1 in
    insmod)
        ./insmod.sh
	exit 0
	;;

    rmmod)
	./rmmod.sh
	exit 0
	;;

    clean)
	clean_modules
	;;

    --help)
	mesg_brown "\n\t./install.sh [ insmod | rmmod ]\n"
	exit 0
	;;
esac



if ! make
then
    mesg_red "Make cdata failed~"
    exit 1
else
    echo -e "\033[33mMake cdata Okay~~"
    ls -l *.ko
    echo -e "\033[0m"
fi

if ! make test
then
    mesg_red "Make test failed~"
    exit 1
fi


[ $# -eq 0 ] && \
    mesg_brown "\t./build.sh [ insmod | rmmod | clean ]\n"
