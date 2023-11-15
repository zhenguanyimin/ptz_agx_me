#ifndef __AI_MAIN_H__
#define __AI_MAIN_H__



#include <opencv2/opencv.hpp>
#include "ptz_control_protocol.h"
#include "alink_track.h"
#include "../yolov8_infer/include/yolov8.h"
#include "../yolov8_infer/include/STrack.h"
#include "../yolov8_infer/include/BYTETracker.h"

#ifdef __cplusplus
extern "C" {
#endif

int ai_alg_run(cv::Mat &image, void *targets);
int ai_alg_run1(cv::Mat &img, void *targets);

//c2选目标
int ai_alg_run2(cv::Mat &img, void *targets);
int ai_alg_run2_id(cv::Mat &img, void *targets);
int ai_alg_run3(cv::Mat &img, void *targets);
int ai_alg_run3_nj(cv::Mat &img, void *targets);

int ai_alg_run7(cv::Mat &img, void *targets);

int32_t ptz_pitch_azimuth(ptz_protocol_msg_t *msg);
int32_t ptz_dev_status(ptz_protocol_msg_t *msg);
int32_t ptz_dev_ext_info_0x08(ptz_protocol_msg_t *msg);
int32_t ptz_target_info(ptz_protocol_msg_t *msg);

void test_upload_target_info(void);
#ifdef __cplusplus
}
#endif

#endif
