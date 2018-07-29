#!/bin/bash
#
# Making device node : /dev/cdata
# Change node permission to 666
#
DEVICE_NODE=/dev/cdata


mesg_brown "Creating device node: ${DEVICE_NODE}"
if ! [ -e ${DEVICE_NODE} ]
then

    sudo mknod $DEVICE_NODE c 121 32
    echo -e "\033[32mMaking device node\033[0m"

    if ! [ -e $DEVICE_NODE ]; then
	echo -e "\033[31mMake cdata failed~\033[0m"
	exit 1
    fi
    sudo chmod 666 $DEVICE_NODE
    ls -l $DEVICE_NODE --color=auto
fi

