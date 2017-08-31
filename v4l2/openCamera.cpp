#include <iostream>
#include <fstream>
#include <linux/videodev2.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <malloc.h>
using namespace std;

struct buffer {  
    void *                  start;  
    size_t                  length;  
};  
unsigned long n_buffers;
struct buffer *buffers;
int file_fd;
int fd;

static int read_frame(void) 
{
    struct v4l2_buffer buf;

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    ioctl(fd, VIDIOC_DQBUF, &buf);

    write(file_fd, buffers[buf.index].start, buffers[buf.index].length);
    ioctl(fd, VIDIOC_QBUF, &buf);

    return 1;
}

int main(void)
{
    // struct v4l2_buffer buf;
    struct v4l2_capability cap;
    struct v4l2_format fmt;
    struct v4l2_requestbuffers req;
    struct v4l2_buffer buf;
    enum v4l2_buf_type type;
    int i;
	int ret;

    file_fd = open("test.jpg", O_RDWR | O_CREAT, 0777);
    fd = open("/dev/video0", O_RDWR);
    // filed_fd = open("test.jpg", O_RDWR | O_CREAT);

    ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
	if (ret){
		cout << "error:" << ret << endl;
		return ret;
	}

    cout << "Driver name:" << cap.driver << "\tCard name:" << cap.card << "\tBus info:"  << cap.bus_info << endl;

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 320;
    fmt.fmt.pix.height = 240;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;

    ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
	if (ret) {
		cout << __func__<< ":" << __LINE__ << "ret is " << ret << endl;
		return ret;
	}

    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    ret = ioctl(fd, VIDIOC_REQBUFS, &req);
	if (ret) {
		cout << __func__<< ":" << __LINE__ << "ret is " << ret << endl;
		return ret;
	}

    buffers = (struct buffer *)calloc(req.count, sizeof(*buffers));

    for (n_buffers = 0; n_buffers < req.count; n_buffers++)
    {
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;

        ret = ioctl(fd, VIDIOC_QUERYBUF, &buf);
		if (ret) {
			cout << __func__<< ":" << __LINE__ << "ret is " << ret << endl;
			return ret;
		}

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
    }

    for (i = 0; i < n_buffers; i++){
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory =  V4L2_MEMORY_MMAP;
        buf.index = i;
        ret = ioctl(fd, VIDIOC_QBUF, &buf);
		if (ret) {
			cout << __func__<< ":" << __LINE__ << "ret is " << ret << endl;
			return ret;
		}
    }

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd, VIDIOC_STREAMON, &type);
	if (ret) {
		cout << __func__<< ":" << __LINE__ << "ret is " << ret << endl;
		return ret;
	}

    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    select(fd + 1, &fds, NULL, NULL, NULL);
    read_frame();

    for (i = 0; i < n_buffers; i++){
        munmap(buffers[i].start, buffers[i].length);
    }

    close(fd);
    close(file_fd);
    cout << "Camera done.\n" << endl;

    return 0;
}
