#########################################################################
# File Name: get_and_trans_img.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Tue 26 Sep 2017 03:38:35 PM CST
#########################################################################
#!/bin/bash
#~/workspace/cross_compile_app/bin/aarch64-linux-gnu-g++ camera_yuyv.cpp -o mtk_yuyv

ssh root@192.168.1.100  "cd ~/;./mtk_yuyv 14"
scp root@192.168.1.100:~/mtk_yuyv0.data .

python ./python/yuyv2png.py mtk_yuyv0.data 1280 800