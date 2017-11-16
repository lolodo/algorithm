#########################################################################
# File Name: i2c.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Wed 25 Oct 2017 07:48:43 PM CST
#########################################################################
#!/bin/bash
make clean

ip=192.168.1.1
interface=`ifconfig | grep enp0s2 | awk '{print $1}'`
echo $interface
sudo ifconfig $interface 192.168.1.10 up

ifconfig $interface
ssh root@$ip "i2cset -f -y 0 0x3f 0x12 0xcf;i2cget -f -y 2 0x48 0x00;i2cget -f -y 2 0x68 0x00"

ssh root@$ip "rmmod max9288"

echo "Start to compile!"
make
echo "Compilation finished!"
scp max9288.ko root@$ip:~/

ssh root@$ip "insmod max9288.ko"
