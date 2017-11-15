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

struct buffer {
	void * start;
	size_t length;
}
struct buffer *buffers;
using namespace std;

int main(void)
{
	struct v4l2_capability cap;
	struct v4l2_format fmt;
	struct v4l2_requestbuffers req;
	struct v4l2_buffer buf;
	enum v4l2_buf_type type;
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
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;

	ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
	if (ret) {
		cout << "[" << __func__ << "] line:" << __LINE__ << " failed:" << ret << endl;
		return ret;
	}

	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	ret = ioctl(fd, VIDIOC_REQBUFS, &req);
	if (ret) {
		cout << "[" << __func__ << "] line:" << __LINE__ << " failed:" << ret << endl;
		return ret;
	}

	buffers = (struct buffer *)calloc(req.count, sizeof(*buffers));

	for (int i = 0; i < req.count; i++) {
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
 
		ret = ioctl(fd, VIDIOC_QUERYBUF, &buf);
		if (ret) {
			cout << "[" << __func__ << "] line:" << __LINE__ << " failed:" << ret << endl;
			return ret;
		}

		buffers[i].length = buf.length;
		buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
	}

	for (i = 0; i < req.count; i++) {
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		ret = ioctl(fd, VIDIOC_QBUF, &buf);
		if (ret) {
			cout << "[" << __func__ << "] line:" << __LINE__ << " failed:" << ret << endl;
			return ret;
		}
	}
	close(fd);

	return 0;
}
