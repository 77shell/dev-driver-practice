#!/bin/bash

. ./VAR.sh


for i in ${KO[@]}
do
    if sudo rmmod $i; then
	mesg_green "Uninstall module: $i"
    else
	mesg_red "Uninstall driver: $i failed~"
    fi
done


