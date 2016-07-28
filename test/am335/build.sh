#!/bin/bash

#
# 2016-7-07
# max.yang@deltaww.com
#

PROJ_ROOT=../..

error_msg()
{
    echo -e "\033[31mErrooooor~ $1\033[0m"
}

declare -a TEST_APP=(
    test
    test-fb-ssd1308
    test-oled-ssd1308
)

copy_files_to_remote()
{
    if ! [ -d $REMOTE_DIR ]; then
	error_msg $REMOTE_DIR
	exit 1
    fi

    #
    # Test applications
    #
    for i in ${TEST_APP[@]}
    do
	! cp $i $REMOTE_DIR && \
	    error_msg $1
    done

    echo -e "\033[33m$REMOTE_DIR\033[0m"
    ls -l $REMOTE_DIR --color
}


. ${PROJ_ROOT}/am335/setup.sh

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
	make V=0
	ls -l --color
	echo -e "\033[33m\n\t./built.sh [ install | clean ]\n\033[0m"
esac
