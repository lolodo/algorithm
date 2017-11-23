#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>

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
        delta = (1000000 + curr_usec - orig_usec) / 1000;
    else
        delta = (curr_usec - orig_usec) / 1000;
	
   // printf("s:%lu us:%lu delta:%lums\n", curr_sec, curr_usec, delta);
    
    printf("delta:%lums\n", delta);
    return delta;
}

