#include <linux/videodev2.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>

#include "avm_common.h"

int dest_port = 8554;
char *dest_address = "127.0.0.1";
int v4l2_device = 14;

uint32_t src_width = 1280;
uint32_t src_height = 3200;

uint32_t dest_width = 1280;
uint32_t dest_height = 800;
float factor = 3/2;

char *src_format = "YUY2";
char *dest_format = "NV12";

int original_image = -1;

int app_debug = 0;

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


int main(int argc, char *argv[])
{
    int ret = 0;
    int snapshot_image = 0;
    int count = 0;
    uint64_t delta = 0;

    libgst_init(argc, argv);

    parse_opts(argc, argv);
    ret = libavm_blacksesame_init();
    if (ret != 0) {
        printf("[%s]%d ret:%d\n", __func__, __LINE__, ret);
        return ret;
    }
    printf("[%s]%d libavm_blacksesame_init ret:%d\n", __func__, __LINE__, ret);

    if (original_image != -1)
        dest_format = src_format;

    gst_appsink_init(); 
    gst_appsrc_init(); 

    char *convert_buff;
    int convert_size;
    int convert_factor = 4;
    if (!strcmp(dest_format, "NV12"))
        convert_factor = 3;
    else if (!strcmp(dest_format, "YUY2"))
        convert_factor = 4;
    convert_size = (dest_width * dest_height * convert_factor) / 2;
    //convert_size = (dest_width * dest_height * 4) / 2;
    convert_buff = (char *)malloc(convert_size);
    if (convert_buff == NULL) {
        printf("[%s]%d no memory\n", __func__, __LINE__);
        return -1;
    }

    printf("[%s]%d convert_buff:%p convert_size:%d\n", __func__, __LINE__, convert_buff, convert_size);

    int src_fd, convert_fd;
    src_fd = open("src.data", O_RDWR | O_CREAT, 0777);
    if (src_fd < 0)
        printf("[%s]%d failed to open src", __func__, __LINE__);

    convert_fd = open("con.data", O_RDWR | O_CREAT, 0777);
    if (convert_fd < 0)
        printf("[%s]%d failed to open src", __func__, __LINE__);

    char *original_copy_buff;
    int original_size = src_width * src_height * 2;
    original_copy_buff  = (char *)malloc(original_size);
    if (original_copy_buff == NULL) {
        printf("[%s]%d no memory for original image\n", __func__, __LINE__);
        return -1;
    }

    while (1) {
        uint64_t curr_sec, curr_usec;
        char *buff;

        get_time_stamp(&curr_sec, &curr_usec);
        int size = pull_buffer(&buff);
        printf("[%s]%d pull buffer ----> ", __func__, __LINE__);
        print_time_diff(curr_sec, curr_usec);

        get_time_stamp(&curr_sec, &curr_usec);
        memcpy(original_copy_buff, buff, original_size);
        printf("[%s]%d copy buffer ----> ", __func__, __LINE__);
        print_time_diff(curr_sec, curr_usec);

        if (size != 0) {
            if (snapshot_image == 1) {
                write(src_fd, buff, size);
            }
            if (app_debug)
                printf("[%s]%d src buff:%p, size:%d, convert_buff:%p, convert_size:%d\n", __func__, __LINE__, 
                        buff, size, convert_buff, convert_size);

            printf("%s: %d -----------convert start----------------\n", __func__, __LINE__);
            //memset(convert_buff, 0, convert_size);
            get_time_stamp(&curr_sec, &curr_usec);
            if (original_image == -1)
                ret = libavm_blacksesame_convert_image(original_copy_buff, size, convert_buff, convert_size);
            delta += print_time_diff(curr_sec, curr_usec);

            count++;
            if ((count % 10) == 0) {
                printf("%s: %d count:%d, total delta:%luus avg delta:%luus \n", __func__, __LINE__,
                        count, delta, delta / count);
            }
            printf("%s: %d -----------convert   end----------------\n", __func__, __LINE__);

            free_pull_buffer();


            if (app_debug)
                printf("[%s]%d convert buff:%p size:%d ret:%d\n", __func__, __LINE__, convert_buff, convert_size, ret);

            if (snapshot_image == 1) {
                write(convert_fd, convert_buff, convert_size);
            }

            if (ret == 0) {
                get_time_stamp(&curr_sec, &curr_usec);
                if (original_image == -1)
                    prepare_buffer(convert_buff, convert_size);
                else
                    prepare_buffer(original_copy_buff + convert_size * original_image, convert_size);
                printf("[%s]%d display image ----> ", __func__, __LINE__);
                print_time_diff(curr_sec, curr_usec);
            }

        }
    }
    close(src_fd);
    close(convert_fd);

    libavm_blacksesame_deinit();
    gst_appsink_fini();
    gst_appsrc_fini();

    return 0;
}
