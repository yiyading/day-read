#!/bin/sh

module="scull"
device="scull"
mode="664"

rm -rf /dev/${device}[0-3]

#major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)
major=30	// major为主设备号，标识驱动

mknod /dev/${device}0 c $major 0
mknod /dev/${device}1 c $major 1
mknod /dev/${device}2 c $major 2
mknod /dev/${device}3 c $major 3

chmod $mode /dev/${device}[0-3]
