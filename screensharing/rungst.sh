#!/bin/sh
PHONEIP=`adb shell ifconfig wlan0|grep "inet addr"|cut -d":" -f2|cut -d" " -f1`
gst-launch-1.0 tcpclientsrc  port=8554 host=${PHONEIP} ! jpegdec ! videoconvert ! autovideosink
#gst-launch-1.0 tcpclientsrc  port=8899 host=${PHONEIP} ! jpegdec ! videoconvert ! autovideosink
