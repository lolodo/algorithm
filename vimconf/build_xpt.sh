export TEMPLATECONF=${PWD}/meta/meta-xpt-bsp/templates/mt2712/xpt-gladius_v1/
source meta/poky/oe-init-build-env

bitbake xpt-core-image  -k 2>&1 | tee build.log
