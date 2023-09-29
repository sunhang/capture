#include "h264_encoder.h"
#include "v4l2_video.h"
#include "common.h"
#include "video_socket.h"

int callback(uint8_t *payload, int i_frame_size) {
    send(client_sock, payload, i_frame_size, 0);
}

int main(void) {
    video_socket_listen_accept();
    v4l2_setup();
    x264_setup();

    unsigned int count = VIDEO_CAPTURE_LOOP;
    while (count-- > 0) {
        // 1秒钟采集15帧
        usleep(1000 * 1000 / FPS);

        // 得到一帧数据
        struct v4l2_buffer *pBufInfo = v4l2_dequeue_buffer();
        void *pData;
        int len;
        v4l2_get_data_address(pBufInfo, &pData, &len);

        // 转换成h264
        uint8_t *nalu = 0;
        int nalu_size = 0;
        yuv_to_nalu(pData, &nalu, &nalu_size);

        // socket发送
        send(client_sock, nalu, nalu_size, 0);

        // 把buffer还给系统
        v4l2_enqueue_buffer(pBufInfo);
    }

    x264_flush_dispose(callback);
    v4l2_dispose();
    video_socket_dispose();

    printf("\n\nDone.\n");
    return 0;
}