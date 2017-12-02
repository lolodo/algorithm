#########################################################################
# File Name: build.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Fri 29 Sep 2017 04:11:06 PM CST
#########################################################################
#!/bin/bash
name=basic-tutorial-$1

gcc $name.c -o  $name.out `pkg-config --cflags --libs gstreamer-1.0`
