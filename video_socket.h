//
// Created by sunhang on 2023/9/23.
//

#ifndef CAPTURE_VIDEO_SOCKET_H
#define CAPTURE_VIDEO_SOCKET_H

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

extern int socket_desc, client_sock;

int video_socket_listen_accept(void);

void video_socket_dispose();

#endif //CAPTURE_VIDEO_SOCKET_H
