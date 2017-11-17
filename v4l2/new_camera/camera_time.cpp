#include <iostream>
#include <fstream>
#include <linux/videodev2.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
using namespace std;

#define PIC_CNT 4
#define PIC_SIZE (1280 * 3200 * 2)

struct buffer {  
    void *                  start;  
    size_t                  length;  
};  
unsigned long n_buffers;
struct buffer *buffers;
int fd;

uint64_t get_time_stamp(uint64_t *sec, uint64_t *usec)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    *sec = tv.tv_sec;
    *usec = tv.tv_usec;
    return 0;
}

uint64_t print_time_diff(uint64_t orig_sec, uint64_t orig_usec)
{
    uint64_t curr_sec, curr_usec, delta;
    get_time_stamp(&curr_sec, &curr_usec);
    if (curr_sec > orig_sec)
        delta = 1000000 + curr_usec - orig_usec;
    else
        delta = curr_usec - orig_usec;
    printf("orig_sec:%lu usec:%lu, curr_sec:%lu usec:%lu delta:%luus\n",
            orig_sec, orig_usec, curr_sec, curr_usec, delta);

    return delta;
}

static int read_frame(void) 
{
    struct v4l2_buffer buf;
	int ret;
	int i;
	char file_name[16];
	int f_id;
	uint64_t sec, usec;
	char *pic_buff[PIC_CNT];
	
	printf("[%s]%d\n", __func__, __LINE__);
	for (i = 0; i < PIC_CNT; i++) {
		pic_buff[i] = (char *)malloc(PIC_SIZE);
		if (pic_buff[i] == NULL) {
			printf("[%s]%d, get the %dth pic_buff failed!\n", __func__, __LINE__, i);
			return -1;
		}
	}

	printf("[%s]%d\n", __func__, __LINE__);

	for (i = 0; i < PIC_CNT; i++) {
		ret = snprintf(file_name, 16, "mtk_yuyv%d.data",i);
		if (ret < 0){
			cout << __func__<< ":" << dec << __LINE__ << "==>>pic "<< i<< endl;
			return ret;
		}

		printf("[%s]%d\n", __func__, __LINE__);
		f_id = open(file_name, O_RDWR | O_CREAT, 0777);
		
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	    buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		get_time_stamp(&sec, &usec);
	    ret = ioctl(fd, VIDIOC_DQBUF, &buf); 
		if (ret) {
			printf("[%s]%d, failed:%d\n", __func__, __LINE__, i);
			return ret;
		}
	
		memcpy(pic_buff[i], buffers[buf.index].start, buffers[buf.index].length);
		print_time_diff(sec, usec);
		usleep(100000);
		printf("[%s]%d\n", __func__, __LINE__);
	    ret = write(f_id, buffers[buf.index].start, buffers[buf.index].length);
		if (ret < 0) {
			printf("[%s]%d, failed!\n", __func__, __LINE__);
			return ret;
		}

	    ret = ioctl(fd, VIDIOC_QBUF, &buf);
		if (ret) {
			printf("[%s]%d, failed!\n", __func__, __LINE__);
			return ret;
	
		}

		close(f_id);
    }

	for (i = 0; i < PIC_CNT; i++) {
		free(pic_buff[i]);
	}

    return 0;
}

int main(int argc, char *argv[])
{
    // struct v4l2_buffer buf;
    struct v4l2_capability cap;
    struct v4l2_format fmt;
    struct v4l2_requestbuffers req;
    struct v4l2_buffer buf;
    enum v4l2_buf_type type;
    int i;
	int ret;
	char dev_name[16];
if (argc !=2) {
		cout << "invalid param!" << endl;
		return argc;
	}

	ret = snprintf(dev_name, 16, "/dev/video%d", atoi(argv[1]));
	if (ret < 0) {
		cout << "Get dev name error!" << endl;
		return ret;
	}

	cout << "dev name is " << dev_name << endl;

    fd = open(dev_name, O_RDWR| O_NONBLOCK, 0);
    // filed_fd = open("test.jpg", O_RDWR | O_CREAT);

    ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
	if (ret){
		cout << "error:" << ret << endl;
		return ret;
	}
	
	struct v4l2_fmtdesc fmtdesc;
	fmtdesc.index = 0;
	//memset(&fmtdesc,0,sizeof(fmtdesc));
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	while (ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc) == 0)
	{
		printf("%s\n", fmtdesc.description);
		fmtdesc.index++;
	}
	
	usleep(1000);
    cout << "Driver name:" << cap.driver << endl;
	cout <<"Card name:" << cap.card <<endl; 
	cout <<"Bus info:"  << cap.bus_info << endl;
    cout << "Driver version:0X"<< hex << cap.version <<endl;
	cout <<"capabilities:0X" << hex << cap.capabilities<< endl;
	cout <<"device_caps:0X" << hex  << cap.device_caps<< endl;

	usleep(1000);
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 1280;
    fmt.fmt.pix.height = 3200;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;

    ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
	if (ret) {
		cout << __func__<< ":" << dec << __LINE__ << "==>>ret is "<< dec << ret << endl;
		return ret;
	}

    req.count = PIC_CNT;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    ret = ioctl(fd, VIDIOC_REQBUFS, &req);
	if (ret) {
		cout << __func__<< ":" << __LINE__ << "ret is " << ret << endl;
		return ret;
	}

    buffers = (struct buffer *)calloc(req.count, sizeof(*buffers));

	cout << __func__<< ":" << dec << __LINE__ << endl;
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

	cout << __func__<< ":" << dec << __LINE__ << endl;
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

	cout << __func__<< ":" << dec << __LINE__ << endl;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd, VIDIOC_STREAMON, &type);
	if (ret) {
		cout << __func__<< ":" << __LINE__ << "ret is " << ret << endl;
		return ret;
	}

	cout << __func__<< ":" << dec << __LINE__ << endl;
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);

	cout << __func__<< ":" << dec << __LINE__ << endl;
    ret = select(fd + 1, &fds, NULL, NULL, NULL);
	if (ret > 0) {
		if (FD_ISSET(fd, &fds)) {
			cout << __func__<< ":" << dec << __LINE__ << endl;
			ret = read_frame();
			if (ret) {
				return ret;
			}
		} else {
			cout << __func__<< ":" << dec << __LINE__ << endl;
		}
	}

	cout << __func__<< ":" << dec << __LINE__ << endl;
    for (i = 0; i < n_buffers; i++){
        munmap(buffers[i].start, buffers[i].length);
    }

	cout << __func__<< ":" << dec << __LINE__ << endl;
    close(fd);
    cout << "Camera done.\n" << endl;

	cout << __func__<< ":" << dec << __LINE__ << endl;
    return 0;
}
