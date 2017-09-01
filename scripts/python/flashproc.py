#!/usr/bin/python
import os

procedures = {
    # product   : fastboot args
    'DEFAULT'   : [['fbWait'],
                    ['fastboot', '-i', '0x0e8d', 'flash', 'EMPTY', 'lk.img'],
                    ['fastboot', '-i', '0x0e8d', 'continue'],
                    ['fbWait'],
                    ['fastboot', '-i', '0x0e8d', 'erase', 'mmc0'],
                    ['fastboot', '-i', '0x0e8d', 'erase', 'mmc0boot0'],
                    ['fastboot', '-i', '0x0e8d', 'flash', 'mmc0boot0', 'MBR_EMMC_BOOT0'],
                    ['fastboot', '-i', '0x0e8d', 'flash', 'BL1', 'lk.img'],
                    ['fastboot', '-i', '0x0e8d', 'flash', 'mmc0', 'MBR_EMMC'],
                    ['fastboot', '-i', '0x0e8d', 'flash', 'TEE1', 'tz.img'],
                    ['fastboot', '-i', '0x0e8d', 'flash', 'BOOTIMG', 'boot.img'],
                    ['fastboot', '-i', '0x0e8d', 'flash', 'ROOTFS', 'rootfs.ext4'],
                    ['fastboot', '-i', '0x0e8d', 'flash', 'STATE', 'state.ext4'] ]
}

userprocedures = {
    # product   : fastboot args
    'DEFAULT'   : [['fbWait'],
                    ['fastboot', '-i', '0x0e8d', 'flash', 'EMPTY', 'lk.img'],
                    ['fastboot', '-i', '0x0e8d', 'continue'],
                    ['fbWait'],
                    ['fastboot', '-i', '0x0e8d', 'flash', 'ROOTFS', 'rootfs.ext4'],
                    ['fastboot', '-i', '0x0e8d', 'flash', 'STATE', 'state.ext4'] ]
}

bootprocedures = {
    # product   : fastboot args
    'DEFAULT'   : [['fbWait'],
                    ['fastboot', '-i', '0x0e8d', 'flash', 'EMPTY', 'lk.img'],
                    ['fastboot', '-i', '0x0e8d', 'continue'],
                    ['fbWait'],
                    ['fastboot', '-i', '0x0e8d', 'erase', 'mmc0boot0'],
                    ['fastboot', '-i', '0x0e8d', 'flash', 'mmc0boot0', 'MBR_EMMC_BOOT0'],
                    ['fastboot', '-i', '0x0e8d', 'flash', 'BL1', 'lk.img'],
                    ['fastboot', '-i', '0x0e8d', 'flash', 'TEE1', 'tz.img'],
                    ['fastboot', '-i', '0x0e8d', 'flash', 'BOOTIMG', 'boot.img'] ]
}

testprocedures = {
    # product   : fastboot args
    'DEFAULT'   : [['fbWait'],
                    ['fastboot', '-i', '0x0e8d', 'flash', 'EMPTY', 'lk.img'],
                    ['fastboot', '-i', '0x0e8d', 'continue'],
                    ['fbWait'],
                    ['fastboot', '-i', '0x0e8d', 'flash', 'UNITEST', 'unitest.ext4'],
                    ['fastboot', '-i', '0x0e8d', 'flash', 'VIDEOFILE', 'videofile.ext4'] ]
}

# return procedure list
def getFlashProc(product):
    try:
        ret = procedures[product.upper()]
        return ret
    except Exception, e:
        return None


def getFlashUserProc(product):
    try:
        ret = userprocedures[product.upper()]
        return ret
    except Exception, e:
        return None

def getFlashBootProc(product):
    try:
        ret = bootprocedures[product.upper()]
        return ret
    except Exception, e:
        return None

def getFlashTestProc(product):
    try:
        ret = testprocedures[product.upper()]
        return ret
    except Exception, e:
        return None
