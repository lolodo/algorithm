1,��ʼ�����ӣ���ʼ����ʱ�����һ��
 int sv_init��
	char * fn_LUT,		// LUT �ļ�������ʹ������·��
	char * fn_gtmap, 	// GTmapping.txt �ļ�������ʹ������·��
	char * fn_ctrlPara, // control parameter �ļ�������ʹ������·��
	int W_In, 			// ���뵥��ͼ�������ֻ����1280
	int H_In, 			// ���뵥��ͼ��ߣ�����ֻ����800
	int W_Out,			// �������ͼ�������ֻ����1280
	int H_Out			// �������ͼ��ߣ�����ֻ����800
	)
	
	����ֵ��
	0 �� ok
	-1�� ��ʼ������
2�� �����ں�������,ÿץ��һ�ε���һ��:
    int sv_convert_image(
        char *src_buff,     // input yuyv data, 
        int src_size,       // Ŀǰֻ���ǣ�1280 * 800 * 4 * 2
        char *dest_buff,    // output YUV, NV12 format
        int dest_size);     // Ŀǰֻ���ǣ�1280 * 800 * 3/2

	����ֵ��
	0 �� ok
	-1�� ��ʼ������
	
3����Դ�ͷź����������˳�ǰ����һ�Σ�
    int sv_deinit();
	
	����ֵ��
	0 �� ok
	-1�� ��ʼ������