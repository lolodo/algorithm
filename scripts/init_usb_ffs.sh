#########################################################################
# File Name: init_usb.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Mon 21 Aug 2017 09:55:01 AM CST
#########################################################################
#!/bin/bash

#Mount ConfigFS and create Gadget
mkdir /config
mount -t configfs none /config
mkdir /config/usb_gadget/g1
cd /config/usb_gadget/g1

#Set default Vendor and Product IDs for now
echo 0x18d1 > idVendor
echo 0x4E26 > idProduct

#Create English strings and add random deviceID
mkdir strings/0x409
echo 0123459876 > strings/0x409/serialnumber

#Update following if you want to
echo "Test" > strings/0x409/manufacturer
echo "Test" > strings/0x409/product

#Create gadget configuration
mkdir configs/c.1
mkdir configs/c.1/strings/0x409
echo "Conf 1" > configs/c.1/strings/0x409/configuration
echo 120 > configs/c.1/MaxPower

#Create Adb FunctionFS function
#And link it to the gadget configuration
stop adbd
mkdir functions/ffs.adb
ln -s /config/usb_gadget/g1/functions/ffs.adb /config/usb_gadget/g1/configs/c.1/ffs.adb

#Start adbd application
mkdir -p /dev/usb-ffs/adb
mount -o uid=0,gid=0 -t functionfs adb /dev/usb-ffs/adb
#start adbd

#Enable the Gadget
#Replace "ci_hdrc.0" with your platform UDC driver as found in /sys/class/udc/
#echo "11271000.usb" > /config/usb_gadget/g1/UDC


