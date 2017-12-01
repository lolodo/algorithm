#########################################################################
# File Name: i2c.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Wed 25 Oct 2017 07:48:43 PM CST
#########################################################################
#!/bin/bash

interface=`ifconfig | grep enp0s20 | awk '{print $1}'`
echo $interface
sudo ifconfig $interface 192.168.1.10 up

ifconfig $interface
gst-launch-1.0 udpsrc address=0.0.0.0 port=8554 caps="application/x-rtp,encoding-name=JPEG,payload=26" ! rtpjpegdepay ! jpegdec ! autovideosink
