#########################################################################
# File Name: killblack.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Thu 23 Nov 2017 07:13:07 PM CST
#########################################################################
#!/bin/bash
kill -9 `ps -aux | grep avm_blacksesame | awk 'NR==1{print $2}'`
