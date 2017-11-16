#########################################################################
# File Name: get_log.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Fri 27 Oct 2017 01:32:17 PM CST
#########################################################################
#!/bin/bash
rm -rf log
ssh root@10.111.160.131 "rm log;dmesg > log"
scp root@10.111.160.131:~/log .
vim log
