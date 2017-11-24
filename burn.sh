#########################################################################
# File Name: /home/fangyuan/workspace/scripts/new_burn.sh
# Author: Alex
# mail: fangyuan.liu@nio.com
# Created Time: Mon 23 Oct 2017 12:20:27 PM CST
#########################################################################
#!/bin/bash
cd /run/user/1000/gvfs/smb-share:server=10.111.136.53,share=hpshare/workspace/yocto_newest/build/tmp/deploy/images/xpt-gladius-v1

fastboot -i 0x0e8d devices
fastboot -i 0x0e8d flash EMPTY lk.img   # Enter Brom fastboot mode
sleep 1
fastboot -i 0x0e8d continue            # Press Download key before you hit Enter, and only release it until LK fastboot mode is on.
echo "release it!"
sleep 2
fastboot -i 0x0e8d erase mmc0boot0
fastboot -i 0x0e8d erase mmc0
fastboot -i 0x0e8d erase nor0
fastboot -i 0x0e8d flash mmc0boot0 MBR_EMMC_BOOT0
fastboot -i 0x0e8d flash nor0 MBR_NOR
fastboot -i 0x0e8d flash mmc0 MBR_EMMC
fastboot -i 0x0e8d flash BL1 lk.img
fastboot -i 0x0e8d flash TEE1 tz.img
fastboot -i 0x0e8d flash BOOTIMG boot.img
fastboot -i 0x0e8d flash ROOTFS rootfs.ext4
fastboot -i 0x0e8d flash STATE state.ext4
# fastboot -i 0x0e8d flash DTBO boot-args.dtbo
fastboot -i 0x0e8d reboot
