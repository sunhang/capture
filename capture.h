//
// Created by sunhang on 2023/9/22.
//

#ifndef CAPTURE_CAPTURE_H
#define CAPTURE_CAPTURE_H

extern void (*process_image)(const void *pBuffer, const int len);
int do_capture_work();

#endif //CAPTURE_CAPTURE_H
