#########################################################################
# File Name: init_usb.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Mon 21 Aug 2017 09:55:01 AM CST
#########################################################################
#!/bin/bash
mkdir /data/misc/adb
mkdir /data/adb
mkdir /dev/socket

chmod 0755 /data/misc/adb
chmod 0700 /data/adb

read -p "Press any key to continue."

#to-do:create socket>>>socket adbd stream 660 system system

echo 0 > /sys/class/android_usb/android0/enable
echo 18d1 > /sys/class/android_usb/android0/idVendor
echo 0001 > /sys/class/android_usb/android0/idProduct
echo adb > /sys/class/android_usb/android0/functions
echo 1 > /sys/class/android_usb/android0/enable


export ANDROID_SOCKET_adbd=3
chmod 777 adb_init
./adb_init
chmod 660 /dev/socket/adbd
./adbd
#todo start adbd



