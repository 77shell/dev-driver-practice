#!/bin/bash

. ./VAR.sh

# Clean kernel message buffer
sudo dmesg -C

./rmmod.sh > /dev/null

for i in ${KO[@]}
do
    ! sudo insmod $i \
	&& mesg_red "Install driver: $i failed~"
done

for i in ${DEVICE_NODES[@]}
do
    if [ ! -e $i ]; then
	mesg_red "No device node, $i"
    else
	! sudo chmod 666 $i && \
	    mesg_red "Change permission of $i failed~"
    fi
done


echo -e "\033[33mInstall modules complete."
lsmod | grep cdata
dmesg | tail -n 40
tree -hfC -H . --du -o dev.html /dev
echo -e "\033[0m"

mesg_green "Checking device nodes...."
for i in ${DEVICE_NODES[@]}
do
    if [ -e "$i" ]; then
	mesg_green "\t$i"
    else
	mesg_red "No $i exists~"
    fi
done

mesg_green "Checking sysfs...."
for i in ${SYS_NODES[@]}
do
    if [ -d "$i" ]; then
	mesg_green "\t$i"
    else
	mesg_red "No $i exists~"
    fi
done


ls /dev/cdata* -l --color=auto
