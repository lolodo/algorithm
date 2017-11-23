#include <linux/videodev2.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>  
#include <errno.h>
#include "avm_common.h"


#define BUFFER_CNT 128 

int dest_port = 8554;
char *dest_address = "127.0.0.1";
int v4l2_device = 14;

uint32_t src_width = 1280;
uint32_t src_height = 720;

uint32_t dest_width = 1280;
uint32_t dest_height = 720;
float factor = 3/2;

unsigned int avm_fps = 0;
unsigned int pic_count = 0;
unsigned int avm_play_time = 0;
uint64_t avm_sec, avm_usec;
pthread_t consumer = 0;  
pthread_t producer = 0;  

char *src_format = "YUY2";
char *dest_format = "YUY2";

int original_image = -1;

int app_debug = 0;

struct avm_buffer_ctrl {
	void *buffer;
	unsigned long w_idx;
	unsigned long r_idx;
	unsigned long depth;
	unsigned long size;
};

struct avm_buffer_ctrl avm_bctl; 

void free_queue(struct avm_buffer_ctrl *ctrl);
static void print_usage(const char *prog)
{
    printf("Usage: %s [options]\n", prog);
    puts(   "  -p --port                specify port to send\n"
            "  -d --dest                specfiy dest address to send\n"
            "  -o --original            specfiy which original camera num to display\n"
            "  -v --v4l2device          specfiy v4l2 device file, 14 for /dev/video14\n"
            "  -h --help                print help info \n");
    exit(1);
}

static void parse_opts(int argc, char *argv[])
{
    while (1) {
        static const struct option lopts[] = {
            { "port", 1, 0, 'p' },
            { "dest", 1, 0, 'd' },
            { "original", 1, 0, 'o' },
            { "v4l2device", 1, 0, 'v' },
            { "help", 0, 0, 'h' },
            { NULL, 0, 0, 0 },
        };
        int c;

        c = getopt_long(argc, argv, "d:ho:p:v:", lopts, NULL);

        if (c == -1)
            break;

        switch (c) {
            case 'p':
                dest_port = atoi(optarg);
                break;
            case 'd':
                dest_address = optarg;
                break;
            case 'v':
                v4l2_device = atoi(optarg);
                break;
            case 'o':
                original_image = atoi(optarg);
                if (original_image < 0 && original_image > 3)
                    original_image = -1;
                break;
            case '?':
            case 'h':
            default:
                print_usage(argv[0]);
                break;

        }
    }

}

void sig_handler(int signo) {
	uint64_t sec, usec;
	int64_t delta;

	if (signo == SIGINT){
		pthread_cancel(producer);
		//pthread_cancel(consumer);
		printf("avm_sec:%lu, avm_usec:%lu\n", avm_sec, avm_usec);
        get_time_stamp(&sec, &usec);
		printf("sec:%lu, usec:%lu\n", sec, usec);
		if (sec > avm_sec) {
			printf("sec > avm_sec!\n");
			if (usec > avm_usec) {
				sec -= avm_sec;
				delta = (usec - avm_usec) / 1000;
			} else {
				sec -= (avm_sec + 1);
				delta = (1000000 + usec - avm_usec) / 1000;
			}
		} else if (sec == avm_sec){
			printf("sec == avm_sec!\n");
			sec = 0;
			delta = (usec - avm_usec) / 1000;
			if (delta < 0) {
				printf("delta < 0!\n");
				return;
			}
		} else {
			printf("sec < avm_sec!\n");
			return;
		}

		printf("===force to quit====\n");
		if (sec > 0) {
			avm_fps = pic_count / (unsigned int)sec;
			printf("avm:count:%d, sec:%lu, delta:%ldms, fps:%u\n", pic_count, sec, delta, avm_fps);
		} else {
			printf("avm:Waiting longer!\n");
		}

		gst_appsink_fini();
		gst_appsrc_fini();
		free_queue(&avm_bctl);
	}
}

int buffer_queue_init(unsigned int num, unsigned int size) {
	void *buffer;
	
	if (num == 0 || size == 0) {
		printf("input error, num:%u, size:%u\n", num, size);
		return -1;
	}

	if (size & 7) {
		printf("size should be aligned:%u!\n", size);
		return -1;
	}

	if (num < 4) {
		printf("buffer number is too small:%u!\n", num);
		return -1;
	}

	buffer = malloc(num * size);
	if (!buffer) {
		printf("malloc buffer failed!\n");
		return -1;
	}

	avm_bctl.buffer = buffer;
	avm_bctl.size = size;
	avm_bctl.w_idx = 0;
	avm_bctl.r_idx = 0;
	avm_bctl.depth = num;

	printf("init succeed!\n");

	return 0;
}

unsigned long check_if_write_available(void)
{
	unsigned long widx, ridx, depth, number;

	widx = avm_bctl.w_idx;
	ridx = avm_bctl.r_idx;
	depth = avm_bctl.depth;

	if (ridx == widx) {
		return depth;
	}

	number = (depth + ridx - widx) % depth;

	return number;
}

int enqueue(void *single_image, unsigned length, struct avm_buffer_ctrl *b_ctrl)
{
	void *start;
	unsigned long widx;
	unsigned long size;

	size = b_ctrl->size;
	if (length > size) {
		printf("enqueue:length > size!\n");
		return -1;
	}

	if (!check_if_write_available()) {
		printf("enqueue:buffer unavailable!\n");
		return -1;
	}

	widx = (b_ctrl->w_idx) % (b_ctrl->depth);
	start = (void *)((unsigned long)b_ctrl->buffer + widx * size);
	memcpy(start, single_image, length);
	b_ctrl->w_idx++;

	return 0;
}

int check_if_read_available(void)
{
	unsigned long widx, ridx, depth, number;

	widx = avm_bctl.w_idx;
	ridx = avm_bctl.r_idx;
	depth = avm_bctl.depth;

	number = (depth + widx - ridx) % depth;

	return number;
}

void *dequeue(struct avm_buffer_ctrl *ctrl, unsigned length)
{
	void *start;
	unsigned long ridx;

	if (length > ctrl->size) {
		printf("dequeue:length(%d) > size(%d)!\n", length, ctrl->size);
		return NULL;
	}

	if (!check_if_read_available()) {
		return NULL;
	}

	ridx = ctrl->r_idx % ctrl->depth;
	start = (void *)((unsigned long)ctrl->buffer + ridx * ctrl->size);
	ctrl->r_idx++;

	return start;
}


void free_queue(struct avm_buffer_ctrl *ctrl)
{
	ctrl->w_idx = 0;
	ctrl->r_idx = 0;
	ctrl->size = 0;
	ctrl->depth = 0;
	free(ctrl->buffer);
}

void *gst_consumer(void *args)
{
	int ret;
	void *buffer;
	struct avm_buffer_ctrl *ctrl;
	unsigned long size;
    uint64_t curr_sec, curr_usec;

	ctrl = (struct avm_buffer_ctrl *)args;
	size = src_width * src_height * 2;
	while(1) {
		if (ctrl->size == 0) {
			return NULL;
		}

		get_time_stamp(&curr_sec, &curr_usec);
		buffer = dequeue(ctrl, size);
		if (buffer == NULL) {
			continue;
		}

		get_time_stamp(&curr_sec, &curr_usec);

		/* simulate convert process */
		usleep(120000);

		printf("consumer:convert ");
		print_time_diff(curr_sec, curr_usec);

		get_time_stamp(&curr_sec, &curr_usec);
		prepare_buffer(buffer, size);
		printf("consumer:display ");
		print_time_diff(curr_sec, curr_usec);
		pic_count++;
	}
}

void *gst_producer(void *args)
{
    uint64_t curr_sec, curr_usec;
    char *buff;
	int size, ret;
	struct avm_buffer_ctrl *ctrl;
	
	ctrl = (struct avm_buffer_ctrl *)args;
	while (1) {
		printf("producer:start to pull\n");
        get_time_stamp(&curr_sec, &curr_usec);
		
        size = pull_buffer(&buff);
		if (!size) {
			printf("[%s]%d size is zero!\n", __func__, __LINE__);
			continue;
		}
		printf("producer:pull end, size:%d ", size);

        print_time_diff(curr_sec, curr_usec);

        get_time_stamp(&curr_sec, &curr_usec);

		//printf("[%s]%d\n", __func__, __LINE__);
		if (buff == NULL) {
			printf("[%s]%d, buffer is null!\n", __func__, __LINE__);
			continue;
		}
	
		printf("producer:start to copy\n");
		ret = enqueue(buff, size, ctrl);
		if (ret) {
			printf("[%s]%d, enqueue failed!\n", __func__, __LINE__);
			continue;
		}

		usleep(40000);
        printf("producer:copy end ");
        print_time_diff(curr_sec, curr_usec);
		free_pull_buffer();
	}
}

int main(int argc, char *argv[])
{
    int ret = 0;
    int original_size;
	struct avm_buffer_ctrl *ctrl;
	
	signal(SIGINT, sig_handler);
    libgst_init(argc, argv);
    parse_opts(argc, argv);

    gst_appsink_init(); 
    gst_appsrc_init(); 

    original_size = src_width * src_height * 2;
	ret = buffer_queue_init(BUFFER_CNT,original_size);
	if (ret) {
		printf("init buffer failed!\n");
		goto release_gst;
	}

	usleep(10000);
	ctrl = &avm_bctl;
	ret = pthread_create(&producer, NULL, gst_producer, &avm_bctl);
	if (ret) {
		printf("create producer failed!\n");
		goto release_buffer;
		
	}
#if 0
	ret = pthread_create(&consumer, NULL, gst_consumer, &avm_bctl);
	if (ret) {
		printf("create consumer failed!\n");
		goto release_buffer;
	}
#endif
    get_time_stamp(&avm_sec, &avm_usec);
	gst_consumer(&avm_bctl);

	return 0;

release_buffer:
	free_queue(ctrl);
	
release_gst:
    gst_appsink_fini();
    gst_appsrc_fini();
    return ret;
}
