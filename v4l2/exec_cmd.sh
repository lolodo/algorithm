#########################################################################
# File Name: i2c.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Wed 25 Oct 2017 07:48:43 PM CST
#########################################################################
#!/bin/bash
#sudo ifconfig enp0s20f0u1u3u2 192.168.1.1 up
~/workspace/cross_compile_app/bin/aarch64-linux-gnu-g++ camera_yuyv.cpp -o mtk_yuyv
scp mtk_yuyv root@192.168.1.1:~/
ssh root@192.168.1.1 "cd /home/root;chmod a+x mtk_yuyv; ./mtk_yuyv 14"
echo "Getting images!"
ssh root@192.168.1.1 "scp root@192.168.1.1:~/mtk_yuyv0.data .;scp root@192.168.1.1:~/mtk_yuyv1.data .;scp root@192.168.1.1:~/mtk_yuyv2.data .;scp root@192.168.1.1:~/mtk_yuyv3.data ."
python ./python/yuyv2png.py mtk_yuyv0.data 1280 720
python ./python/yuyv2png.py mtk_yuyv1.data 1280 720
python ./python/yuyv2png.py mtk_yuyv2.data 1280 720
python ./python/yuyv2png.py mtk_yuyv3.data 1280 720
