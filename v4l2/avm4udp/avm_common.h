#ifndef AVM_COMMON_H
#define AVM_COMMON_H

#include <stdint.h>

#define AVM_CONFIG_PATH "/etc/avm"

int libgst_init(int argc, char *argv[]);
int gst_appsrc_init();
int gst_appsrc_fini();
int prepare_buffer(char *buff_send, uint32_t size);
int gst_appsink_init();
int gst_appsink_fini();
int pull_buffer(char **buff);

uint64_t print_time_diff(uint64_t orig_sec, uint64_t orig_usec);
uint64_t get_time_stamp(uint64_t *sec, uint64_t *usec);

extern int dest_port;
extern char *dest_address;
extern int v4l2_device;
extern int app_debug;

extern uint32_t src_width;
extern uint32_t src_height;
extern uint32_t dest_width;
extern uint32_t dest_height;

extern char *src_format;
extern char *dest_format;

extern int libavm_blacksesame_init();
extern int libavm_blacksesame_convert_image(char *src_buff, int src_size, char *dest_buff,
        int dest_size);
extern int libavm_blacksesame_deinit();

#endif
