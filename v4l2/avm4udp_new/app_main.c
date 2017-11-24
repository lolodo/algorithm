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

//#define BOARD_DEV
#define USB_CAMERA 
#define BUFFER_CNT 4 
#define BUFFER_WATERLINE 3

int dest_port = 8554;
char *dest_address = "127.0.0.1";
int v4l2_device = 14;

char *src_format = "YUY2";
#ifdef BOARD_DEV
uint32_t src_width = 1280;
uint32_t src_height = 3200;

uint32_t dest_width = 1280;
uint32_t dest_height = 800;
char *dest_format = "NV12";
#else
#ifdef USB_CAMERA
uint32_t src_width = 640;
uint32_t src_height = 480;

uint32_t dest_width = 640;
uint32_t dest_height = 480;
#else
uint32_t src_width = 1280;
uint32_t src_height = 720;

uint32_t dest_width = 1280;
uint32_t dest_height = 720;
#endif
char *dest_format = "YUY2";
#endif
float factor = 3/2;

unsigned int free_flag = 0;
unsigned int avm_fps = 0;
unsigned int pic_count = 0;
unsigned int avm_play_time = 0;
uint64_t avm_sec, avm_usec;
pthread_t consumer = 0;  
pthread_t producer = 0;  

int original_image = -1;
int app_debug = 0;

enum avm_thread_mode {
	SINGLE_THREAD,
	MULTI_THREAD,
	MODE_BUTTOM
};

struct avm_buffer_ctrl {
	void *buffer;
	unsigned long w_idx;
	unsigned long r_idx;
	unsigned long depth;
	unsigned long size;
	int convert_delay;

#ifdef BOARD_DEV
	void *convert_buffer;
	unsigned long convert_size;
#endif
	pthread_mutex_t mutex;
	pthread_cond_t consume_cond;
	pthread_cond_t produce_cond;

	enum avm_thread_mode thread_mode;
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
			"  -m --mode                specfiy thread mode\n"
			"  -l --delay				specfiy convert delay(ms)\n"
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
			{ "mode", 1, 0, 'm' },
			{ "delay", 1, 0, 'l' },
            { "help", 0, 0, 'h' },
            { NULL, 0, 0, 0 },
        };
        int c;

        c = getopt_long(argc, argv, "d:ho:p:v:m:l:", lopts, NULL);

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
                if (original_image < 0 || original_image > 3)
                    original_image = -1;
                break;
			case 'm':
				avm_bctl.thread_mode = atoi(optarg);
				if (avm_bctl.thread_mode >= MODE_BUTTOM) {
					printf("thread mode is error:%d\n", avm_bctl.thread_mode);
					avm_bctl.thread_mode = 0;
				}
				break;
			case 'l':
				avm_bctl.convert_delay = atoi(optarg);
				printf("convert_delay:%d\n", avm_bctl.convert_delay);
				if (avm_bctl.convert_delay < 0) {
					avm_bctl.convert_delay = 0;
				}
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
	struct avm_buffer_ctrl *ctrl; 

	ctrl = &avm_bctl;
	if (signo == SIGINT){
		ctrl->size = 0;

		if (ctrl->thread_mode == MULTI_THREAD) {
			pthread_cond_signal(&(ctrl->produce_cond));
			pthread_cond_signal(&(ctrl->consume_cond));
		}
		
		//pthread_cancel(producer);
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
				goto out;
			}
		} else {
			printf("sec < avm_sec!\n");
			goto out;
		}

		printf("====force to quit====\n");
		if (sec > 0) {
			avm_fps = (pic_count - 5) / (unsigned int)sec;
			printf("====avm:count:%d, sec:%lu, delta:%ldms, fps:%u====\n", pic_count, sec, delta, avm_fps);
		} else {
			printf("avm:Waiting longer!\n");
		}
out:
		if (ctrl->thread_mode == MULTI_THREAD) {
			printf("[%s]====kill producer and consumer!====\n", __func__);
			pthread_cancel(producer);
			pthread_cancel(consumer);
			printf("[%s]====free mutex and cond!====\n", __func__);
			//pthread_mutex_destroy(&(ctrl->mutex));
			//pthread_cond_destroy(&(ctrl->produce_cond));
			//pthread_cond_destroy(&(ctrl->consume_cond));
		}

		if (free_flag == 0){
			printf("[%s]====free gst/avm/buffer!====\n", __func__);
			gst_appsrc_fini();
			gst_appsink_fini();
#ifdef BOARD_DEV
			libavm_blacksesame_deinit();
#endif
			free_queue(ctrl);
		}
		
		free_flag = 1;
	}
}

int buffer_queue_init(unsigned int num, unsigned int size) 
{
	void *buffer;
	unsigned long convert_factor;
	
	if (num == 0 || size == 0) {
		printf("input error, num:%u, size:%u\n", num, size);
		return -1;
	}

	if (size & 7) {
		printf("size should be aligned:%u!\n", size);
		return -1;
	}

	if (num < 2) {
		printf("buffer number is too small:%u!\n", num);
		return -1;
	}

	buffer = malloc(num * size);
	if (!buffer) {
		printf("malloc buffer failed!\n");
		return -1;
	}	

#ifdef BOARD_DEV
	if (!strcmp(dest_format, "NV12")) {
		convert_factor = 3;
	} else if (!strcmp(dest_format, "YUY2"){
		convert_factor = 4;
	}

	avm_bctl.convert_size = (dest_width * dest_height * convert_factor) / 2;
	avm_bctl.convert_buffer = malloc(avm_bctl.convert_size);
	if (avm_bctl.convert_buffer == NULL) {
		printf("convert_buff unavailable!\n");
		return -1;
	}
#endif

	avm_bctl.buffer = buffer;
	avm_bctl.size = size;
	avm_bctl.w_idx = 0;
	avm_bctl.r_idx = 0;
	avm_bctl.depth = num;
	
	if (avm_bctl.thread_mode == MULTI_THREAD) {
		pthread_mutex_init(&(avm_bctl.mutex), NULL);
		pthread_cond_init(&(avm_bctl.produce_cond), NULL);
		pthread_cond_init(&(avm_bctl.consume_cond), NULL);
	}

	free_flag = 0;
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

int enqueue(void *single_image, unsigned length, struct avm_buffer_ctrl *ctrl)
{
	void *start;
	unsigned long widx;
	unsigned long size;
	int ret = 0;

	size = ctrl->size;
	if (length > size) {
		printf("enqueue:length > size!\n");
		return -1;
	}

	if (check_if_write_available() < BUFFER_WATERLINE) {
		pthread_mutex_lock(&(ctrl->mutex));
		free_pull_buffer();
		length = pull_buffer(&single_image);
		if (length > size) {
			ret = -1;
		}

		if (single_image == NULL) {
			ret = -1 ;
		}

		pthread_cond_wait(&(ctrl->consume_cond), &(ctrl->mutex));
		pthread_mutex_unlock(&(ctrl->mutex));

		if (ret) {
			return ret;
		}
	}

	pthread_mutex_lock(&(ctrl->mutex));

	widx = (ctrl->w_idx) % (ctrl->depth);
	start = (void *)((unsigned long)ctrl->buffer + widx * size);
	memcpy(start, single_image, length);
	ctrl->w_idx++;
	
	/* Simulate memory copy */
//	usleep(40000);

	pthread_cond_signal(&(ctrl->produce_cond));
	pthread_mutex_unlock(&(ctrl->mutex));
	printf("enqueue:widx:%ld\n", ctrl->w_idx);

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
		printf("dequeue:length(%u) > size(%lu)!\n", length, ctrl->size);
		return NULL;
	}

	if (!check_if_read_available()) {
		pthread_mutex_lock(&(ctrl->mutex));
		pthread_cond_wait(&(ctrl->produce_cond), &(ctrl->mutex));
		pthread_mutex_unlock(&(ctrl->mutex));
		return NULL;
	}
	
	pthread_mutex_lock(&(ctrl->mutex));

	ridx = ctrl->r_idx % ctrl->depth;
	start = (void *)((unsigned long)ctrl->buffer + ridx * ctrl->size);
	ctrl->r_idx++;

	pthread_cond_signal(&(ctrl->consume_cond));
	pthread_mutex_unlock(&(ctrl->mutex));

	printf("dequeue:ridx:%ld\n", ctrl->r_idx);
	
	return start;
}


void free_queue(struct avm_buffer_ctrl *ctrl)
{
	if (ctrl->depth == 0){
		return;
	}

	ctrl->w_idx = 0;
	ctrl->r_idx = 0;
	ctrl->size = 0;
	free(ctrl->buffer);

#ifdef BOARD_DEV
	free(ctrl->convert_buffer);
	ctrl->convert_size = 0;
#endif

	ctrl->depth = 0;
}

void *gst_consumer(void *args)
{
	int ret;
	void *buffer;

#ifdef BOARD_DEV
	void *convert_buffer;
	unsigned long convert_size;
#endif

	struct avm_buffer_ctrl *ctrl;
	unsigned long size;
    uint64_t curr_sec, curr_usec;

	ctrl = (struct avm_buffer_ctrl *)args;

#ifdef BOARD_DEV
	convert_size = ctrl->convert_size;
	convert_buffer = ctrl->convert_buffer;
#endif

	size = src_width * src_height * 2;

    get_time_stamp(&avm_sec, &avm_usec);
	while(1) {
		if (ctrl->size == 0) {
			break;
		}

		get_time_stamp(&curr_sec, &curr_usec);
		buffer = dequeue(ctrl, size);
		if (buffer == NULL) {
			continue;
		}

		printf("[%s]:dequeue buffer ", __func__);
		print_time_diff(curr_sec, curr_usec);
		get_time_stamp(&curr_sec, &curr_usec);
#ifdef BOARD_DEV
		libavm_blacksesame_convert_image(buffer, size, convert_buffer, convert_size);	

		printf("[%s]:convert buffer ", __func__);
		print_time_diff(curr_sec, curr_usec);

		get_time_stamp(&curr_sec, &curr_usec);
		prepare_buffer(convert_buffer, convert_size);
		printf("[%s]:display buffer ", __func__);
		print_time_diff(curr_sec, curr_usec);
#else
		/* Simulate converting */
		if(ctrl->convert_delay) {
			usleep(1000 * ctrl->convert_delay);
		}

		printf("[%s]:convert buffer ", __func__);
		print_time_diff(curr_sec, curr_usec);

		get_time_stamp(&curr_sec, &curr_usec);
		prepare_buffer(buffer, size);
		printf("[%s]:display buffer ", __func__);
		print_time_diff(curr_sec, curr_usec);
#endif
		pic_count++;

		if (pic_count < 5) {
			get_time_stamp(&avm_sec, &avm_usec);
		
		}
	}
	pthread_exit(0);
}

void *gst_producer(void *args)
{
    uint64_t curr_sec, curr_usec;
    char *buff;
	int size, ret;
	static int count = 0;
	struct avm_buffer_ctrl *ctrl;
	
	ctrl = (struct avm_buffer_ctrl *)args;
	while (1) {
		if (ctrl->size == 0) {
			break;
		}

        get_time_stamp(&curr_sec, &curr_usec);
        size = pull_buffer(&buff);
		if (!size) {
			printf("[%s]%d size is zero!\n", __func__, __LINE__);
			continue;
		}
		
		printf("[%s]:pull buffer ", __func__);
        print_time_diff(curr_sec, curr_usec);
        get_time_stamp(&curr_sec, &curr_usec);
		if (buff == NULL) {
			printf("[%s]%d, buffer is null!\n", __func__, __LINE__);
			continue;
		}
	
		ret = enqueue(buff, size, ctrl);
		if (ret) {
			free_pull_buffer();
			printf("[%s]%d, enqueue failed!\n", __func__, __LINE__);
			continue;
		}

		printf("[%s]:enqueue buffer ", __func__);
        print_time_diff(curr_sec, curr_usec);
		free_pull_buffer();
		count++;
	}

	pthread_exit(0);
}

void single_thread_run(struct avm_buffer_ctrl *ctrl)
{
	unsigned long src_size, dst_size;
	void *src_buffer;
	void *dst_buffer;
    uint64_t curr_sec, curr_usec;

#ifdef BOARD_DEV
	void *output_buffer;
	unsigned long convert_size;
	convert_size = ctrl->convert_size;
	output_buffer = ctrl->convert_buffer;
#endif

	dst_buffer= ctrl->buffer;
	dst_size = ctrl->size;
	while (1) {
		if (ctrl->size == 0) {
			break;
		}

		get_time_stamp(&curr_sec, &curr_usec);
		src_size = pull_buffer(&src_buffer);
		if (!src_size) {
			printf("[%s]src_size is zero!\n", __func__);
			continue;
		}

		if (!src_buffer) {
			printf("[%s]src_buffer is NULL!\n", __func__);
			continue;
		}

		if (src_size > dst_size) {
			printf("[%s]%d, src size is %lu, dst size is %lu\n", __func__, __LINE__, src_size, dst_size);
			src_size = dst_size;
		}
		
		printf("[%s]pull buffer ", __func__);
		print_time_diff(curr_sec, curr_usec);
		
		get_time_stamp(&curr_sec, &curr_usec);

#ifdef BOARD_DEV
		memcpy(dst_buffer, src_buffer, src_size);
#endif
		printf("[%s]copy buffer ", __func__);
		print_time_diff(curr_sec, curr_usec);

		get_time_stamp(&curr_sec, &curr_usec);
		if (ctrl->convert_delay) {
			usleep(1000 * ctrl->convert_delay);
		}

#ifdef BOARD_DEV
		if (original_image != -1) {
			prepare_buffer(buffer + convert_size * original_image, convert_size);
		} else {
			ret = libavm_blacksesame_convert_image(dst_buffer, dst_size, output_buffer, convert_size);
			if (ret) {
				printf("convert failed\n");
				continue;
			}
			prepare_buffer(output_buffer, convert_size);
		}

#else
		prepare_buffer(src_buffer, src_size);
#endif
		printf("[%s]convert buffer ", __func__);
		print_time_diff(curr_sec, curr_usec);

		free_pull_buffer();
		pic_count++;
		if (pic_count < 5) {
			get_time_stamp(&avm_sec, &avm_usec);
		}
	}
}

int main(int argc, char *argv[])
{
    int ret = 0;
    int original_size;
	struct avm_buffer_ctrl *ctrl;

	printf("[%s]init start!\n", __func__);
	ctrl = &avm_bctl;
	memset(ctrl, 0, sizeof(struct avm_buffer_ctrl));

	signal(SIGINT, sig_handler);
    libgst_init(argc, argv);
    parse_opts(argc, argv);

    gst_appsink_init(); 
    gst_appsrc_init(); 

#ifdef BOARD_DEV
	ret = libavm_blacksesame_init();
	if (ret != 0) {
		printf("[%s]%d ret:%d\n", __func__, __LINE__, ret);
		goto release_gst;
	}
#endif

    original_size = src_width * src_height * 2;
	ret = buffer_queue_init(BUFFER_CNT,original_size);
	if (ret) {
		printf("init buffer failed!\n");
		goto release_black;
	}

	printf("[%s]init end!\n", __func__);
	if (ctrl->thread_mode == SINGLE_THREAD ){
		printf("[%s]single_thread_run!\n", __func__);
		single_thread_run(ctrl);
	} else {
		printf("[%s]multi_thread_run!\n", __func__);
		ret = pthread_create(&producer, NULL, gst_producer, ctrl);
		if (ret) {
			printf("create producer failed!\n");
			goto release_buffer;
		}

		ret = pthread_create(&consumer, NULL, gst_consumer, ctrl);
		if (ret) {
			printf("create producer failed!\n");
			goto release_buffer;
		}	

		/* block threads */
		pthread_join(producer, NULL);
		pthread_join(consumer, NULL);
	}

	return 0;

release_buffer:
	if (free_flag == 0){
		printf("[%s]====free buffer====\n", __func__);
		free_queue(ctrl);
	}

release_black:
#ifdef BOARD_DEV
	if (free_flag == 0){
		printf("[%s]====free avm====\n", __func__);
		libavm_blacksesame_deinit();
	}
#endif

release_gst:
	if (free_flag == 0){
		printf("[%s]====free gst====\n", __func__);
	    gst_appsink_fini();
		gst_appsrc_fini();
	}

	free_flag = 1;
    return ret;
}
