1,初始化环视，初始化的时候调用一次
 int sv_init（
	char * fn_LUT,		// LUT 文件名，请使用完整路径
	char * fn_gtmap, 	// GTmapping.txt 文件名，请使用完整路径
	char * fn_ctrlPara, // control parameter 文件名，请使用完整路径
	int W_In, 			// 输入单幅图像宽，现在只能是1280
	int H_In, 			// 输入单幅图像高，现在只能是800
	int W_Out,			// 输出单幅图像宽，现在只能是1280
	int H_Out			// 输出单幅图像高，现在只能是800
	)
	
	返回值：
	0 ： ok
	-1： 初始化错误
2， 环视融合主函数,每抓拍一次调用一次:
    int sv_convert_image(
        char *src_buff,     // input yuyv data, 
        int src_size,       // 目前只能是：1280 * 800 * 4 * 2
        char *dest_buff,    // output YUV, NV12 format
        int dest_size);     // 目前只能是：1280 * 800 * 3/2

	返回值：
	0 ： ok
	-1： 初始化错误
	
3，资源释放函数，程序退出前调用一次：
    int sv_deinit();
	
	返回值：
	0 ： ok
	-1： 初始化错误