#########################################################################
# File Name: run.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: 2017年11月22日 星期三 12时05分34秒
#########################################################################
#!/bin/bash
./gen_vim.sh
if [ ! -d "./build" ]; then
  mkdir ./build
fi


cd ./build
cmake ../
make clean
make
if [ $? -ne 0 ]
then
   echo "make error"
else
   echo "make success!"

   gst-launch-1.0 udpsrc address=0.0.0.0 port=8554 caps="application/x-rtp,encoding-name=JPEG,payload=26" ! rtpjpegdepay ! jpegdec ! autovideosink &
   ./avm_blacksesame -p 8554 -d 127.0.0.1 -m 0 -v 0
   #./avm_blacksesame -p 8554 -d 127.0.0.1 -v 0
fi

