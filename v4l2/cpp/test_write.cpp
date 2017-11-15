#include <iostream>
#include <fstream>
#include <linux/videodev2.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <malloc.h>
#include <stdlib.h>
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

int main(int argc, char *argv[])
{
	char string[] = "hello, world!";
	
    file_fd = open("hello.txt", O_RDWR | O_CREAT, 0777);
    write(file_fd, string, 13);

	close(file_fd);

    return 0;
}
