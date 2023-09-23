//
// Created by sunhang on 2023/9/23.
//

#ifndef CAPTURE_VIDEOSOCKET_H
#define CAPTURE_VIDEOSOCKET_H
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

extern void (*transport_data)(const int client_sock);

int video_socket(void);
#endif //CAPTURE_VIDEOSOCKET_H
