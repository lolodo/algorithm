#########################################################################
# File Name: i2c.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Wed 25 Oct 2017 07:48:43 PM CST
#########################################################################
#!/bin/bash
#sudo ifconfig enp0s20f0u1u3u2 $ip0 up
#ssh root@$ip "reboot"
#sleep 20
#echo "set network card!!"
ip=$1

#ssh root@$ip "echo mode 6 0 > /sys/devices/platform/soc/1000b000.pinctrl/mt_gpio"

#ssh root@$ip "echo out 4 1 > /sys/devices/platform/soc/1000b000.pinctrl/mt_gpio"
#ssh root@$ip "echo out 5 1 > /sys/devices/platform/soc/1000b000.pinctrl/mt_gpio"
#ssh root@$ip "echo out 6 1 > /sys/devices/platform/soc/1000b000.pinctrl/mt_gpio"
#ssh root@$ip "echo out 7 1 > /sys/devices/platform/soc/1000b000.pinctrl/mt_gpio"
#ssh root@$ip "echo out 8 1 > /sys/devices/platform/soc/1000b000.pinctrl/mt_gpio"
#ssh root@$ip "echo out 9 1 > /sys/devices/platform/soc/1000b000.pinctrl/mt_gpio"


ssh root@$ip "cat /sys/devices/platform/soc/10217000.mipicsi0/mipi_reg | grep 134"

echo "0x48 0x27"
ssh root@$ip "i2cget -f -y 2 0x48 0x27"
