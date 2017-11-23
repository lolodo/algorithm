#########################################################################
# File Name: gen_vim.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: 2017年11月17日 星期五 11时45分24秒
#########################################################################
#!/bin/bash
find ${PWD} -name "*.[ch]" -o -name "*.cpp" > cscope.files
cscope -bR -i cscope.files
ctags -R
