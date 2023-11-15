#ifndef __PTZ_APP_H__
#define __PTZ_APP_H__

// #define DEFAULT_RTSP_PTZ "rtsp://10.240.34.120:8554/mystream"
#define DEFAULT_RTSP_PTZ "rtsp://192.168.1.4:554/channel=0,stream=0"

#define DEFAULT_PTZ_READ_FRAME_TIME 20

#ifdef __cplusplus
extern "C" {
#endif

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <arpa/inet.h>

#define PTZ_CAPTURE_TIMEOUT_VALUE   1000 //500ms
#define PTZ_STREAM_ADDR_MAX_LEN	256

typedef struct videostreaminfo
{
    void* image;  // 一帧图像数据
    uint32_t width;
    uint32_t height;
    uint64_t rtp_timestamp;
    uint64_t framecount;
}videostreaminfo_t;

int load_ptz(void);
int relase_ptz_resource(void);

#ifdef __cplusplus
}
#endif

#endif
