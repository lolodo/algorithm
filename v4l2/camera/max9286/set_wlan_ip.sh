#########################################################################
# File Name: set_wlan_ip.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Thu 02 Nov 2017 11:57:16 AM CST
#########################################################################
#!/bin/bash
wpa_supplicant -iwlan0 -c/etc/wpa_supplicant.conf &
dhclient wlan0
ifconfig
