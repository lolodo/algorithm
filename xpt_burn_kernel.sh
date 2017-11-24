#########################################################################
# File Name: newnew.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: 2017年11月20日 星期一 13时51分11秒
#########################################################################
#!/bin/bash
cd /run/user/1000/gvfs/smb-share:server=10.111.136.53,share=hpshare/workspace/yocto_newest/build/tmp/deploy/images/xpt-gladius-v1/

fastboot -i 0x0e8d flash BOOTIMG boot.img
fastboot -i 0x0e8d reboot
