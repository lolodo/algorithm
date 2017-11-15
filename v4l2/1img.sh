#########################################################################
# File Name: get_and_trans_img.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Tue 26 Sep 2017 03:38:35 PM CST
#########################################################################
#!/bin/bash
#~/workspace/cross_compile_app/bin/aarch64-linux-gnu-g++ camera_yuyv.cpp -o mtk_yuyv
scp root@192.168.1.1:~/mtk_yuyv0.data .
scp root@192.168.1.1:~/mtk_yuyv1.data .
scp root@192.168.1.1:~/mtk_yuyv2.data .
scp root@192.168.1.1:~/mtk_yuyv3.data .

python ./python/yuyv2png.py mtk_yuyv0.data 1280 3200
python ./python/yuyv2png.py mtk_yuyv1.data 1280 3200
python ./python/yuyv2png.py mtk_yuyv2.data 1280 3200
python ./python/yuyv2png.py mtk_yuyv3.data 1280 3200
