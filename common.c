//
// Created by sunhang on 2023/9/22.
//

#include "common.h"
#include <sys/time.h>
#include <malloc.h>

static char *time_tag;
static long begin_time_in_milliseconds;

/**
 * Wrapper around ioctl calls.
 */
int xioctl(int fd, int request, void *arg) {
    int r;

    do {
        r = ioctl(fd, request, arg);
    } while (-1 == r && EINTR == errno);

    return r;
}

void begin_mark_timestemp(const char *tag) {
    time_tag = tag;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    begin_time_in_milliseconds = tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void end_mark_timestamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long end_time_in_milliseconds = tv.tv_sec * 1000 + tv.tv_usec / 1000;

    printf("%s time:%ldms\n", time_tag, end_time_in_milliseconds - begin_time_in_milliseconds);
}