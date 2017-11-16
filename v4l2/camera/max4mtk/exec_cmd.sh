#########################################################################
# File Name: i2c.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Wed 25 Oct 2017 07:48:43 PM CST
#########################################################################
#!/bin/bash
make clean

ssh root@10.111.253.64 "rmmod max92"

echo "Start to compile!"
make
echo "Compilation finished!"
scp max92.ko root@10.111.253.64:~/

ssh root@10.111.253.64 "insmod max92.ko"
