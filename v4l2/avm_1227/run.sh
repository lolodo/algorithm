#########################################################################
# File Name: run.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: 2017年11月22日 星期三 12时05分34秒
#########################################################################
#!/bin/bash
./up_usb_net.sh
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
   #ssh root@192.168.1.2 "kill -9 `ps -aux | grep avm_blacksesame | awk 'NR==1{print $2}'`"
   ssh root@192.168.1.2 "kill -9 `ps -aux | grep avm_blacksesame | awk '{print $2}'`"
   scp avm_blacksesame root@192.168.1.2:~/
   ssh root@192.168.1.2 "cd ~/;./avm_blacksesame -p 8554 -d 192.168.1.10"
fi

