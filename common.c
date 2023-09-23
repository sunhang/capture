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

int yuv422_to_420(uint8_t *yuv422, uint8_t *yuv420, int width, int height) {
    int ynum = width * height;
    int i, j, k = 0;
    //得到Y分量
    for (i = 0; i < ynum; i++) {
        yuv420[i] = yuv422[i * 2];
    }
    //得到U分量
    for (i = 0; i < height; i++) {
        if ((i % 2) != 0)continue;
        for (j = 0; j < (width / 2); j++) {
            if ((4 * j + 1) > (2 * width))break;
            yuv420[ynum + k * 2 * width / 4 + j] = yuv422[i * 2 * width + 4 * j + 1];
        }
        k++;
    }
    k = 0;
    //得到V分量
    for (i = 0; i < height; i++) {
        if ((i % 2) == 0)continue;
        for (j = 0; j < (width / 2); j++) {
            if ((4 * j + 3) > (2 * width))break;
            yuv420[ynum + ynum / 4 + k * 2 * width / 4 + j] = yuv422[i * 2 * width + 4 * j + 3];

        }
        k++;
    }

    return 1;
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