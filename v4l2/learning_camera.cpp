/*************************************************************************
    > File Name: learning_camera.cpp
    > Author: Alex
    > Mail: fangyuan.liu@nio.com 
    > Created Time: 2017年08月31日 星期四 19时26分08秒
 ************************************************************************/

#include <iostream>
#include <linux/videodev2.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

using namespace std;

int main(void)
{
	struct v4l2_capability cap;
	struct v4l2_format fmt;
	int fd;
	int ret;

	fd = open("/dev/video0", O_RDWR);

	ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
	if (ret) {
		cout << "[" << __func__ << "] line:" << __LINE__ << " failed:" << ret << endl;
		return ret;
	}

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = 320;
	fmt.fmt.pix.height = 240;
	fmt.fmt.pix.field = 

	close(fd);

	return 0;
}
