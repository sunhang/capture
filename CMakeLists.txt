cmake_minimum_required(VERSION 3.25)
project(capture C)

set(CMAKE_C_STANDARD 11)

add_executable(capture main.c common.h v4l2_video.c h264_encoder.c common.c h264_encoder.h v4l2_video.h video_socket.h video_socket.c common.c)

target_link_libraries(capture x264)