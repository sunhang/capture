//
// Created by sunhang on 2023/9/22.
//

#ifndef CAPTURE_TO_H264_H
#define CAPTURE_TO_H264_H

#include <stdint.h>

int setup_h264();

int to_h264(const void *bytes, uint8_t **payload, int *p_frame_size);

int finish_h264(int (*callback)(uint8_t *payload, int i_frame_size));

#endif //CAPTURE_TO_H264_H
