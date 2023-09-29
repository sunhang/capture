//
// Created by sunhang on 2023/9/22.
//

#ifndef CAPTURE_H264_ENCODER_H
#define CAPTURE_H264_ENCODER_H

#include <stdint.h>

int x264_setup();

void yuv_to_nalu(const void *yuvBuffer, uint8_t **nalu, int *nalu_size);

int yu12_to_nalu(const void *bytes, uint8_t **payload, int *p_frame_size);

int x264_flush_dispose(int (*callback)(uint8_t *payload, int i_frame_size));

int yuyv_to_yu12(uint8_t *yuv422, uint8_t *yuv420, int width, int height);

#endif //CAPTURE_H264_ENCODER_H
