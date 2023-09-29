//
// Created by sunhang on 2023/9/22.
//

#ifndef CAPTURE_V4L2_VIDEO_H
#define CAPTURE_V4L2_VIDEO_H

struct v4l2_buffer *v4l2_dequeue_buffer(void);

void v4l2_get_data_address(struct v4l2_buffer *pBuffer, void **start, int *len);

void v4l2_enqueue_buffer(struct v4l2_buffer *pBuffer);

int v4l2_setup();

int v4l2_dispose();

#endif //CAPTURE_V4L2_VIDEO_H
