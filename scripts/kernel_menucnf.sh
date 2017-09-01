export TEMPLATECONF=${PWD}/meta/base/conf/mt2712/auto2712m1v1-ivi_agl

source meta/poky/oe-init-build-env
cd ./build

bitbake -c menuconfig virtual/kernel -f -v
