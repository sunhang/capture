#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include "common.h"
#include "v4l2_video.h"

static const char DEVICE[] = "/dev/video0";

int fd;
struct {
    void *start;
    size_t length;
} *buffers;
unsigned int num_buffers;
struct v4l2_requestbuffers reqbuf = {0};

void (*process_image)(const void *pBuffer, const int len) = NULL;

static void init_mmap(void) {
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory = V4L2_MEMORY_MMAP;
    reqbuf.count = 2;
    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &reqbuf)) {
        perror("VIDIOC_REQBUFS");
        exit(errno);
    }

    if (reqbuf.count < 2) {
        printf("Not enough buffer memory\n");
        exit(EXIT_FAILURE);
    }

    buffers = calloc(reqbuf.count, sizeof(*buffers));
    assert(buffers != NULL);

    num_buffers = reqbuf.count;

    // Create the buffer memory maps
    struct v4l2_buffer buffer;
    for (unsigned int i = 0; i < reqbuf.count; i++) {
        memset(&buffer, 0, sizeof(buffer));
        buffer.type = reqbuf.type;
        buffer.memory = V4L2_MEMORY_MMAP;
        buffer.index = i;

        // Note: VIDIOC_QUERYBUF, not VIDIOC_QBUF, is used here!
        if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buffer)) {
            perror("VIDIOC_QUERYBUF");
            exit(errno);
        }

        buffers[i].length = buffer.length;
        buffers[i].start = mmap(
                NULL,
                buffer.length,
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                fd,
                buffer.m.offset);

        if (MAP_FAILED == buffers[i].start) {
            perror("mmap");
            exit(errno);
        }
    }
}

static void init_device() {
    struct v4l2_fmtdesc fmtdesc = {0};
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmtdesc.index = FORMAT_INDEX;

    // Use the format in FORMAT_INDEX
    if (-1 == xioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc)) {
        perror("VIDIOC_ENUM_FMT");
        exit(errno);
    }
    printf("\nUsing format: %s\n", fmtdesc.description);

    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    fmt.fmt.pix.width = VIDEO_WIDTH;
    fmt.fmt.pix.height = VIDEO_HEIGHT;
    fmt.fmt.pix.pixelformat = fmtdesc.pixelformat;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)) {
        perror("VIDIOC_S_FMT");
        exit(errno);
    }

    char format_code[5];
    strncpy(format_code, (char *) &fmt.fmt.pix.pixelformat, 5);
    printf(
            "Set format:\n"
            " Width: %d\n"
            " Height: %d\n"
            " Pixel format: %s\n"
            " Field: %d\n\n",
            fmt.fmt.pix.width,
            fmt.fmt.pix.height,
            format_code,
            fmt.fmt.pix.field);

    init_mmap();
}

static void start_capturing(void) {
    enum v4l2_buf_type type;

    struct v4l2_buffer buffer;
    for (unsigned int i = 0; i < num_buffers; i++) {
        /* Note that we set bytesused = 0, which will set it to the buffer length
         * See
         * - https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-qbuf.html?highlight=vidioc_qbuf#description
         * - https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/buffer.html#c.v4l2_buffer
         */
        memset(&buffer, 0, sizeof(buffer));
        buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory = V4L2_MEMORY_MMAP;
        buffer.index = i;

        // Enqueue the buffer with VIDIOC_QBUF
        if (-1 == xioctl(fd, VIDIOC_QBUF, &buffer)) {
            perror("VIDIOC_QBUF");
            exit(errno);
        }
    }

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == xioctl(fd, VIDIOC_STREAMON, &type)) {
        perror("VIDIOC_STREAMON");
        exit(errno);
    }
}

static void stop_capturing(void) {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type)) {
        perror("VIDIOC_STREAMOFF");
        exit(errno);
    }
}

/**
 * Readout a frame from the buffers.
 */
static struct v4l2_buffer *read_frame(void) {
    struct v4l2_buffer *pBuffer = malloc(sizeof(struct v4l2_buffer));
    memset(pBuffer, 0, sizeof(*pBuffer));
    pBuffer->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    pBuffer->memory = V4L2_MEMORY_MMAP;

    // Dequeue a buffer
    if (-1 == xioctl(fd, VIDIOC_DQBUF, pBuffer)) {
        switch (errno) {
            case EAGAIN:
                // No buffer in the outgoing queue
                return 0;
            case EIO:
                // fall through
            default:
                perror("VIDIOC_DQBUF");
                exit(errno);
        }
    }

    assert(pBuffer->index < num_buffers);

    return pBuffer;
}

void v4l2_get_data_address(struct v4l2_buffer *pBuffer, void **start, int *len) {
    *start = buffers[pBuffer->index].start;
    *len = buffers[pBuffer->index].length;
}

void v4l2_enqueue_buffer(struct v4l2_buffer *pBuffer) {
    // Enqueue the buffer again
    if (-1 == xioctl(fd, VIDIOC_QBUF, pBuffer)) {
        free(pBuffer);
        perror("VIDIOC_QBUF");
        exit(errno);
    }

    free(pBuffer);
}

/**
 * Poll the device until it is ready for reading.
 *
 * See https://www.gnu.org/software/libc/manual/html_node/Waiting-for-I_002fO.html
 */
struct v4l2_buffer *v4l2_dequeue_buffer(void) {
    fd_set fds;
    struct timeval tv;
    int r;
    for (;;) {
        // Clear the set of file descriptors to monitor, then add the fd for our device
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        // Set the timeout
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        /**
         * Arguments are
         * - number of file descriptors
         * - set of read fds
         * - set of write fds
         * - set of except fds
         * - timeval struct
         *
         * According to the man page for select, "nfds should be set to the highest-numbered file
         * descriptor in any of the three sets, plus 1."
         */
        r = select(fd + 1, &fds, NULL, NULL, &tv);

        if (-1 == r) {
            if (EINTR == errno)
                continue;

            perror("select");
            exit(errno);
        }

        if (0 == r) {
            fprintf(stderr, "select timeout\n");
            exit(EXIT_FAILURE);
        }
        struct v4l2_buffer *pBuffer;
        if (pBuffer = read_frame()) {
            // Go to next iterartion of fhe while loop; 0 means no frame is ready in the outgoing queue.
            return pBuffer;
        }
    }
}

int v4l2_setup() {
    // Open the device file
    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror(DEVICE);
        return errno;
    }

    init_device();

    start_capturing();
}

int v4l2_dispose() {
    stop_capturing();

    // Cleanup
    for (unsigned int i = 0; i < reqbuf.count; i++)
        munmap(buffers[i].start, buffers[i].length);
    free(buffers);
    close(fd);

    return 0;
}