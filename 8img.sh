#########################################################################
# File Name: get_and_trans_img.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Tue 26 Sep 2017 03:38:35 PM CST
#########################################################################
#!/bin/bash
#~/workspace/cross_compile_app/bin/aarch64-linux-gnu-g++ camera_yuyv.cpp -o mtk_yuyv
rm mtk_yuyv0.data
./up_usb_net.sh
ssh root@192.168.1.2  "cd ~/;./mtk_yuyv 14"
scp root@192.168.1.2:~/mtk_yuyv1.data .

python yuyv2png_4.py mtk_yuyv1.data 1280 3200 $1