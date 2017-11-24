#########################################################################
# File Name: i2c.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Wed 25 Oct 2017 07:48:43 PM CST
#########################################################################
#!/bin/bash
gst-launch-1.0 udpsrc address=0.0.0.0 port=8554 caps="application/x-rtp,encoding-name=JPEG,payload=26" ! rtpjpegdepay ! jpegdec ! autovideosink
