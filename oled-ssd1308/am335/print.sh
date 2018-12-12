#!/bin/bash

cat /sys/class/oled/rotate
echo 180 > /sys/class/oled/rotate
