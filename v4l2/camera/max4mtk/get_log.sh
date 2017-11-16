#########################################################################
# File Name: get_log.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Fri 27 Oct 2017 01:32:17 PM CST
#########################################################################
#!/bin/bash
rm -rf log
ssh root@192.168.1.1 "rm log;dmesg > log"
scp root@192.168.1.1:~/log .
vim log
