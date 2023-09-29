//
// Created by sunhang on 2023/9/22.
//

#ifndef CAPTURE_COMMON_H
#define CAPTURE_COMMON_H

#include <sys/ioctl.h>
#include <errno.h>
#include <asm-generic/int-ll64.h>
#include <stdint.h>

#define VIDEO_WIDTH 640
#define VIDEO_HEIGHT 480
#ifdef __x86_64__
// todo 在不同机器上不一定是1
#define FORMAT_INDEX 1
#else
#define FORMAT_INDEX 0
#endif
#define I_CSP X264_CSP_I420
#define FPS  15
#define VIDEO_CAPTURE_LOOP 2048

/**
 * Wrapper around ioctl calls.
 */
int xioctl(int fd, int request, void *arg);
void begin_mark_timestemp(const char* tag);
void end_mark_timestamp();

#endif //CAPTURE_COMMON_H
