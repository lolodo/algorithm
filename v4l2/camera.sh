#########################################################################
# File Name: camera.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Mon 25 Sep 2017 10:31:51 AM CST
#########################################################################
#!/bin/bash
#ssh-keygen -f "/home/fangyuan/.ssh/known_hosts" -R 192.168.1.1
~/workspace/cross_compile_app/bin/aarch64-linux-gnu-g++ camera_mmu_dgb.cpp -o mtk 
scp mtk root@192.168.1.1:~/
