#########################################################################
# File Name: i2c.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Wed 25 Oct 2017 07:48:43 PM CST
#########################################################################
#!/bin/bash
make clean
sudo ifconfig enp0s20f0u4u3u2 192.168.1.10 up
#ssh root@192.168.1.1 "reboot"
#sleep 20
#echo "set network card!!"
ssh root@192.168.1.1 "i2cset -f -y 0 0x3f 0x12 0xcf; echo out 8 1 > /sys/devices/platform/soc/1000b000.pinctrl/mt_gpio;i2cget -f -y 2 0x48 0x00;i2cget -f -y 2 0x68 0x00"

ssh root@192.168.1.1 "rmmod max9288"

echo "Start to compile!"
make
echo "Compilation finished!"
scp max9288.ko root@192.168.1.1:~/

ssh root@192.168.1.1 "insmod max9288.ko"
