#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

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
    //uint64_t trans_time = 0;
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
