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
//#define USB_CAMERA 
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

unsigned int avm_fps = 0;
unsigned int pic_count = 0;
unsigned int avm_play_time = 0;
uint64_t avm_sec, avm_usec;
pthread_t consumer = 0;  
pthread_t producer = 0;  

int original_image = -1;
int app_debug = 0;

struct avm_buffer_ctrl {
	void *buffer;
	unsigned long w_idx;
	unsigned long r_idx;
	unsigned long depth;
	unsigned long size;

#ifdef BOARD_DEV
	void *convert_buffer;
	unsigned long convert_size;
#endif
	pthread_mutex_t mutex;
	pthread_cond_t consume_cond;
	pthread_cond_t produce_cond;
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
		avm_bctl.size = 0;
		
		pthread_cond_signal(&(avm_bctl.produce_cond));
		pthread_cond_signal(&(avm_bctl.consume_cond));
		
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

		printf("===force to quit====\n");
		if (sec > 0) {
			avm_fps = pic_count / (unsigned int)sec;
			printf("============avm:count:%d, sec:%lu, delta:%ldms, fps:%u============\n", pic_count, sec, delta, avm_fps);
		} else {
			printf("avm:Waiting longer!\n");
		}
out:
		pthread_mutex_destroy(&(avm_bctl.mutex));
		pthread_cond_destroy(&(avm_bctl.produce_cond));
		pthread_cond_destroy(&(avm_bctl.consume_cond));
		
		gst_appsrc_fini();
		gst_appsink_fini();
		free_queue(&avm_bctl);
#ifdef BOARD_DEV
		libavm_blacksesame_deinit();
#endif
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
	avm_bctl.convert_size = (dest_width * dest_height * 3) / 2;
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
	avm_bctl.depth = num;

	pthread_mutex_init(&(avm_bctl.mutex), NULL);
	pthread_cond_init(&(avm_bctl.produce_cond), NULL);
	pthread_cond_init(&(avm_bctl.consume_cond), NULL);

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
	ctrl->w_idx = 0;
	ctrl->r_idx = 0;
	ctrl->size = 0;
	ctrl->depth = 0;
	free(ctrl->buffer);

#ifdef BOARD_DEV
	free(ctrl->convert_buffer);
	ctrl->convert_size = 0;
#endif
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
	

	printf("consumer:start to run!\n");
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

		get_time_stamp(&curr_sec, &curr_usec);

#ifdef BOARD_DEV
		libavm_blacksesame_convert_image(buffer, size, convert_buffer, convert_size);	

		printf("consumer:convert ");
		print_time_diff(curr_sec, curr_usec);

		get_time_stamp(&curr_sec, &curr_usec);
		prepare_buffer(convert_buffer, convert_size);
		printf("consumer:display ");
		print_time_diff(curr_sec, curr_usec);
		pic_count++;
#else

		printf("consumer:convert ");

		/* Simulate converting */
		//usleep(120000);

		print_time_diff(curr_sec, curr_usec);

		get_time_stamp(&curr_sec, &curr_usec);
		prepare_buffer(buffer, size);
		printf("consumer:display ");
		print_time_diff(curr_sec, curr_usec);
		pic_count++;

		if (pic_count < 5) {
			get_time_stamp(&avm_sec, &avm_usec);
		}
#endif
	}

	pthread_exit(0);
}

void *gst_producer(void *args)
{
    uint64_t curr_sec, curr_usec;
    uint64_t sec, usec;
    char *buff;
	int size, ret;
	static int count = 0;
	struct avm_buffer_ctrl *ctrl;
	
	ctrl = (struct avm_buffer_ctrl *)args;
	while (1) {
		if (ctrl->size == 0) {
			break;
		}

		printf("producer:start to pull\n");
        get_time_stamp(&curr_sec, &curr_usec);
        get_time_stamp(&sec, &usec);
		
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
			free_pull_buffer();
			printf("[%s]%d, enqueue failed!\n", __func__, __LINE__);
			continue;
		}

        printf("producer:copy end ");
        print_time_diff(curr_sec, curr_usec);
		free_pull_buffer();
		count++;
	}

	pthread_exit(0);
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

	ctrl = &avm_bctl;
	ret = pthread_create(&producer, NULL, gst_producer, &avm_bctl);
	if (ret) {
		printf("create producer failed!\n");
		goto release_buffer;
		
	}

	ret = pthread_create(&consumer, NULL, gst_consumer, &avm_bctl);
	if (ret) {
		printf("create producer failed!\n");
		goto release_buffer;
	}	

	/* block threads */
	pthread_join(producer, NULL);
	pthread_join(consumer, NULL);

	return 0;

release_buffer:
	free_queue(ctrl);

release_black:
#ifdef BOARD_DEV
	libavm_blacksesame_deinit();
#endif

release_gst:
    gst_appsink_fini();
    gst_appsrc_fini();
    return ret;
}
