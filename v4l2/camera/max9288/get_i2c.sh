#########################################################################
# File Name: get_i2c_addr.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Fri 27 Oct 2017 03:56:28 PM CST
#########################################################################
#!/bin/bash

echo $1
for((i = 0; i < 0xff; i++))
do
	echo $i;i2cget -f -y 2 $1 $i
done
