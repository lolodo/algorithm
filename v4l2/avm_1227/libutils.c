#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>
extern int avm_print_all;
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

	if (!avm_print_all) {
		return 0;
	}
	
    get_time_stamp(&curr_sec, &curr_usec);
    if (curr_sec > orig_sec)
        delta = (1000000 + curr_usec - orig_usec) / 1000;
    else
        delta = (curr_usec - orig_usec) / 1000;
   
	if (orig_sec == 0) {
		printf("\tdelta:%lus, %lums\n", (curr_sec - orig_sec), delta);
	} else {
		printf("\tdelta:%lums\n", delta);
	}
    
    return delta;
}

