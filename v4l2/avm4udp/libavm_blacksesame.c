#include "stdio.h"
#include "avm_common.h"
#include "libavm_blacksesame.h"

#define BLACKSESAME_CONFIG_PATH AVM_CONFIG_PATH "/blacksesame"

int libavm_blacksesame_init()
{
    char *fn_LUT = BLACKSESAME_CONFIG_PATH "/LUT_birdeye.txt";
    char *fn_gtmap = BLACKSESAME_CONFIG_PATH "/GTmapping.txt";
    char * fn_ctrlPara = BLACKSESAME_CONFIG_PATH "/parameter.txt";
    int W_In = 1280;
    int H_In = 800;
    int W_Out = 1280;
    int H_Out = 800;
    int ret = sv_init(fn_LUT, fn_gtmap, fn_ctrlPara,
            W_In,
            H_In,
            W_Out,
            H_Out);
    return ret;
}

int libavm_blacksesame_convert_image(char *src_buff,
        int src_size,
        char *dest_buff,
        int dest_size)
{
    if (src_buff == NULL || dest_buff == NULL ||
            src_size == 0 || dest_size == 0)
        return -1;

    if (app_debug)
        printf("[%s]%d src buff:%p dest_buff:%p src_size:%d, dest_size:%d\n", __func__, __LINE__,
                src_buff, dest_buff, src_size, dest_size);
    return sv_convert_image(
            src_buff,
            src_size,
            dest_buff,
            dest_size);
}

int libavm_blacksesame_deinit()
{
    return sv_deinit();
}
