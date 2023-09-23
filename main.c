#include "to_h264.h"
#include "capture.h"
#include "common.h"
#include "videosocket.h"
#include <stdio.h>
#include <memory.h>
#include <malloc.h>

FILE *p = NULL;
int client_sock;

void process_image_impl(const void *pBuffer, const int len) {
    uint8_t *payload = 0;
    int frame_size = 0;

#ifndef __x86_64__
    to_h264(pBuffer, &payload, &frame_size);
#else
    int size = (int) (VIDEO_WIDTH * VIDEO_HEIGHT * 1.5f);
    uint8_t *yuv420 = malloc(sizeof(uint8_t) * size);
    memset(yuv420, '\0', sizeof(uint8_t) * size);
    yuv422_to_420(pBuffer, yuv420, VIDEO_WIDTH, VIDEO_HEIGHT);
    to_h264(yuv420, &payload, &frame_size);
    free(yuv420);
#endif
    begin_mark_timestemp("send");
    send(client_sock, payload, frame_size, 0);
    end_mark_timestamp();
}

int callback(uint8_t *payload, int i_frame_size) {
    fputc('.', stdout);
    fflush(stdout);
    fwrite(payload, 1, i_frame_size, p);
}

void transport_data_impl(const int arg_client_sock) {
    if (setup_h264() != 0) {
        return;
    }

    client_sock = arg_client_sock;
    process_image = process_image_impl;
    do_capture_work();
    finish_h264(callback);
}

int main(void) {
    transport_data = transport_data_impl;
    video_socket();
    return 0;
}