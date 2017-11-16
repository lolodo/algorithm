#########################################################################
# File Name: get_i2c_addr.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Fri 27 Oct 2017 03:56:28 PM CST
#########################################################################
#!/bin/bash
for((i = 3; i < 0x77; i++))
do
	echo $i;i2cget -f -y 2 $i 0x1e
done
