#!/bin/bash

#
# 2016-7-07
# max.yang@deltaww.com
#
#

REMOTE_DIR=~/remote/tpsbuserver-prj/GigaController/FW/modules
declare -a KO_FILES=(cdata.ko cdata_plat_dev.ko)
declare -a TEST_APP=(test)
declare -a SCRIPT_FILES=(insmod.sh rmmod.sh)

error_msg()
{
    echo -e "\033[31mErrooooor~ $1\033[0m"
}

copy_files_to_remote()
{
    if ! [ -d $REMOTE_DIR ]; then
	error_msg $REMOTE_DIR
	exit 1
    fi

    #
    # Kernel object
    #
    for i in ${KO_FILES[@]}
    do
	! cp ../$i $REMOTE_DIR && \
	    error_msg $i
    done

    #
    # User program
    #
    for i in ${TEST_APP[@]}; do 
	! cp $i $REMOTE_DIR && \
	    error_msg $i
    done

    #
    # Scripts for target
    #
    for i in ${SCRIPT_FILES[@]};
    do
	! cp $i $REMOTE_DIR && \
	    error_msg $i
    done
    ls -l $REMOTE_DIR --color
    
    exit 0
}


case $1 in
    install)
	copy_files_to_remote
	exit 0
	;;

    clean)
	make clean
	exit 0
	;;

    *)
	. ./setup.sh
	make
	make test
esac