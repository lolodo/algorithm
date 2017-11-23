#########################################################################
# File Name: run.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: 2017年11月22日 星期三 12时05分34秒
#########################################################################
#!/bin/bash
cd ./build
cmake ../
make
./avm_blacksesame -p 8554 -d 127.0.0.1 -v 0
