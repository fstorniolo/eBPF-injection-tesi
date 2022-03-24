#!/bin/sh

set -ex
module="newdev"
device="newdev"



# Setup. (ins kernel module, create device file at /dev/newdev)
sudo insmod driver.ko
sudo ./mknoddev.sh newdev
#major=$(awk "\\$2=  =\"$module\" {print \\$1}" /proc/devices)
#mknod /dev/${device}0 c $major 0

