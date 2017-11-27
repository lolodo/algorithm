#########################################################################
# File Name: run.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: 2017年11月22日 星期三 12时05分34秒
#########################################################################
#!/bin/bash

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
fi

