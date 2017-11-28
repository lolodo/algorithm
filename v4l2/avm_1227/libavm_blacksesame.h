#ifndef _SV_INTERFACE_H
#define _SV_INTERFACE_H


#ifdef __cplusplus
extern "C" {
#endif

    // ret  0   : okay
    //      -1  : fail 
    int sv_convert_image(
        char *src_buff,     // input yuyv data, 
        int src_size,       // 1280 * 800 * 4 * 2
        char *dest_buff,    // output YUV, NV12 format
        int dest_size);      // dest size // 1280 * 800 * 3/2

    // ret  0   : okay
    //      -1  : fail 
    int sv_init(
	char * fn_LUT,		// LUT FileName
	char * fn_gtmap, 	// GTmapping.txt FileName
	char * fn_ctrlPara, // control parameter FileName 
	int W_In, 			// 1280
	int H_In, 			// 800
	int W_Out,			// 1280
	int H_Out			// 800
	);
    // ret  0   : okay
    //      -1  : fail 
    int sv_deinit();

#ifdef __cplusplus
}
#endif


#endif
