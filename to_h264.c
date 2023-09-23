#include <stdio.h>
#include "to_h264.h"
#include <x264.h>
#include "common.h"

#define FAIL_IF_ERROR(cond, ...)          \
    do                                    \
    {                                     \
        if (cond)                         \
        {                                 \
            fprintf(stderr, __VA_ARGS__); \
            return -1;                    \
        }                                 \
    } while (0

static x264_t *h;
static int luma_size;
static int chroma_size;
static int i_frame = 0;
static x264_picture_t pic;
static x264_nal_t *nal;
static int i_nal;
static x264_picture_t pic_out;


int setup_h264() {
    x264_param_t param;

    /* Get default params for preset/tuning */
    if (x264_param_default_preset(&param, "ultrafast", "zerolatency") < 0) {
        return -1;
    }

    /* Configure non-default params */
    param.i_csp = I_CSP;
    param.i_width = VIDEO_WIDTH;
    param.i_height = VIDEO_HEIGHT;
    param.b_vfr_input = 0;
    param.b_repeat_headers = 1;
    param.b_annexb = 1;
    param.i_fps_num = FPS; // 分子
    param.i_fps_den = 1;   // 分母

    /* Apply profile restrictions. */
    if (x264_param_apply_profile(&param, "high") < 0) {
        return -1;
    }

    x264_picture_init(&pic);

//    if (x264_picture_alloc(&pic, param.i_csp, param.i_width, param.i_height) < 0) {
//        return -1;
//    }

    h = x264_encoder_open(&param);
    if (!h) {
//        x264_picture_clean(&pic);
        return -1;
    }

    luma_size = VIDEO_WIDTH * VIDEO_HEIGHT;
    chroma_size = luma_size / 4;

    return 0;
}

int to_h264(const void *bytes, uint8_t **payload, int *p_frame_size) {
    int i_frame_size;

    pic.img.plane[0] = bytes;
    pic.img.plane[1] = bytes + luma_size;
    pic.img.plane[2] = bytes + luma_size + chroma_size;
    pic.img.i_plane = 3;

    pic.img.i_csp = X264_CSP_I420;
    pic.img.i_stride[0] = VIDEO_WIDTH;
    pic.img.i_stride[1] = VIDEO_WIDTH / 2;
    pic.img.i_stride[2] = VIDEO_WIDTH / 2;

    pic.i_pts = i_frame++;
    i_frame_size = x264_encoder_encode(h, &nal, &i_nal, &pic, &pic_out);
    *p_frame_size = i_frame_size;
    if (i_frame_size < 0) {
        return -1;
    } else if (i_frame_size) {
        *payload = nal->p_payload;
        return 0;
    }
}

int finish_h264(int (*callback)(uint8_t *payload, int i_frame_size)) {
    /* Flush delayed frames */
    while (x264_encoder_delayed_frames(h)) {
        int i_frame_size = x264_encoder_encode(h, &nal, &i_nal, NULL, &pic_out);
        if (i_frame_size < 0) {
            return -1;
        } else if (i_frame_size) {
            callback(nal->p_payload, i_frame_size);
        }
    }

    x264_encoder_close(h);
//    x264_picture_clean(&pic);
    return 0;
}
