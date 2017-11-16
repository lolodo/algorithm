#########################################################################
# File Name: i2c.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Wed 25 Oct 2017 07:48:43 PM CST
#########################################################################
#!/bin/bash
make clean
#sudo ifconfig enp0s20f0u1u3u2 10.111.161.740 up
#ssh root@10.111.161.74 "reboot"
#sleep 20
#echo "set network card!!"
ssh root@10.111.161.74 "i2cset -f -y 0 0x3f 0x12 0xcf; echo out 8 1 > /sys/devices/platform/soc/1000b000.pinctrl/mt_gpio;i2cget -f -y 2 0x48 0x00;i2cget -f -y 2 0x68 0x00"

ssh root@10.111.161.74 "echo out 6 1 > /sys/devices/platform/soc/1000b000.pinctrl/mt_gpio;echo out 142 1 > /sys/devices/platform/soc/1000b000.pinctrl/mt_gpio"

ssh root@10.111.161.74 "echo out 138 1 > /sys/devices/platform/soc/1000b000.pinctrl/mt_gpio;echo out 142 1 > /sys/devices/platform/soc/1000b000.pinctrl/mt_gpio"

ssh root@10.111.161.74 "rmmod max92"

echo "Start to compile!"
make
echo "Compilation finished!"
scp max92.ko root@10.111.161.74:~/

ssh root@10.111.161.74 "insmod max92.ko"
