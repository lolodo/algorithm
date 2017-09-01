export TEMPLATECONF=${PWD}/meta/base/conf/mt2712/auto2712m1v1-ivi_agl
source meta/poky/oe-init-build-env
bitbake virtual/kernel -c compile -f
bitbake virtual/kernel -f
