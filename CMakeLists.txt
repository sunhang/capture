cmake_minimum_required(VERSION 3.25)
project(capture C)

set(CMAKE_C_STANDARD 11)

add_executable(capture main.c save.c save.h common.h capture.c to_h264.c common.c to_h264.h capture.h read.h read.c videosocket.h videosocket.c common.c)

target_link_libraries(capture x264)