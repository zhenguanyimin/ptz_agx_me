#include "ptz_app.h"
extern "C" {
#include "../../srv/eth_link/eth_link_ptz.h"
#include "../../srv/log/skyfend_log.h"
#include "../../common/tools/skyfend_tools.h"
}

#include "jetson-utils/videoSource.h"
#include "jetson-utils/logging.h"
#include "jetson-utils/commandLine.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <queue>
#include "ai_main.h"

using cv::Mat;
using std::array;
using std::cout;
using std::endl;
using std::ifstream;
using std::ios;
using std::ofstream;
using std::string;
using std::vector;
using namespace cv;

struct ptz_ctrl {
	pthread_t 	ptz_tid;
	pthread_mutex_t 	mutex;
	bool stop_capture;
	bool stop_stream;
	videoSource* input;
	cv::VideoCapture stream;
	std::queue<cv::Mat> q;
	char rtsp_stream_addr[PTZ_STREAM_ADDR_MAX_LEN];
};
static struct ptz_ctrl ptzobj = {0};

#if 0
static void* ptz_camera_stream_thread(void *arg) 
{
	struct ptz_ctrl *ptz_p = (struct ptz_ctrl *)arg;
	void* image = NULL;
	int status = 0;
	std::string rtsp = DEFAULT_RTSP_PTZ;

	ptz_p->stop_capture = false;
	ptz_p->stop_stream = false;

	// ptz_p->stream = cv::VideoCapture(rtsp, cv::CAP_FFMPEG);
	return NULL;
	do {
		ptz_p->stream.open(rtsp);
		// CAP_FFMPEG：使用ffmpeg解码
		if (!ptz_p->stream.isOpened()) {
			// std::cout << "open stream fail" << rtsp << std::endl;
			skyfend_log_error("open stream fail");
			ptz_p->stream.release();
			skyfend_msleep(500);
			continue;
		}
		double rate = ptz_p->stream.get(CAP_PROP_FPS);
		// std::cout << "open stream rate is " << rate << std::endl;
		skyfend_log_info("open stream rate is %f", rate);

		while(!ptz_p->stop_capture) {
			skyfend_msleep(500);
		}
		if(ptz_p->stream.isOpened()) {
			ptz_p->stream.release();
		}
	}while(!ptz_p->stop_stream);
	
	return NULL;
}

#endif






static void* ptz_camera_stream_thread(void *arg) 
{
	struct ptz_ctrl *ptz_p = (struct ptz_ctrl *)arg;
	void* image = NULL;
	int status = 0;
	int ret = -1;

	ptz_p->stop_capture = false;
	ptz_p->stop_stream = false;
	
	cv::VideoCapture stream;
	std::string rtsp = ptz_p->rtsp_stream_addr;
	cv::Mat frame;
	int count = 0;

	do {
		// skyfend_log_info("open stream....");
		stream.open(rtsp);
		if (!stream.isOpened()) {
			skyfend_msleep(10*1000);
			count++;
			skyfend_log_info("open fail, count = %d", count);
			if(count > DEFAULT_PTZ_READ_FRAME_TIME) {
				count = 0;
			}
			stream.release();
			continue;
		}
		double rate = stream.get(CAP_PROP_FPS);
		while(!ptz_p->stop_capture) {
			// skyfend_log_info("read stream....");

			if (!stream.read(frame)) {
				skyfend_log_info("read fail, count = %d", count);
				stream.release();
				break;
			}
			// cv::cvtColor(frame, frame, cv::COLOR_RGBA2BGR);
			ret = pthread_mutex_lock(&(ptz_p->mutex));
			if(0 != ret) {
				skyfend_log_error("%s:%d> get lock fail = %d", __FUNCTION__, __LINE__, ret);
				continue;
			}

			if(ptz_p->q.size() > 4) {
					ptz_p->q.pop();
			}
			ptz_p->q.push(frame);

			pthread_mutex_unlock(&(ptz_p->mutex));
			count = 0;

			// flag++;
			// if(1 < flag && flag < 100) {
			//         char filename[64] = { 0};
			//         sprintf(filename, "test_f%d.jpg", flag);
			//         cv::imwrite(filename, img);
			// }
#if 0
			// cv::resize(img, img, cv::Size(720, 640));
			cv::imshow("test", img);
			cv::waitKey(1);

			//  tracking(img, model, box);  // add your alg interface here.
			// skyfend_log_debug("frame count %ld timesmap = %lu\n", ptz_p->input->GetFrameCount(), ptz_p->input->GetLastTimestamp());
#endif
		}
		skyfend_log_warn("%s: %d> stop loop read picture", __FUNCTION__, __LINE__);

	}while(!ptz_p->stop_stream);
	skyfend_log_warn("%s: %d> exit", __FUNCTION__, __LINE__);

	return NULL;
}

static int32_t ptz_lense_ext_info_0x0c(ptz_protocol_msg_t *msg)
{
	ptz_msg_lense_ext_info_t *lense_ext = (ptz_msg_lense_ext_info_t *)msg->msg; 

	if(NULL == lense_ext) {
		skyfend_log_info("%s:%d msg is null\n", __FUNCTION__, __LINE__);
		return 0;
	}

	skyfend_log_debug("photoelectricity_no = %d", lense_ext->photoelectricity_no);
	skyfend_log_debug("cur_status_timesmap = %lu", lense_ext->cur_status_timesmap);
	skyfend_log_debug("visable_horizatal_angle = %d", lense_ext->visable_horizatal_angle);
	skyfend_log_debug("visable_vertical_angle = %d", lense_ext->visable_vertical_angle);
	skyfend_log_debug("thermal_horization_angle = %d", lense_ext->thermal_horization_angle);
	skyfend_log_debug("thermal_vertical_angle = %d", lense_ext->thermal_vertical_angle);
	skyfend_log_debug("visable_zoom = %d", lense_ext->visable_zoom);
	skyfend_log_debug("thermal_zoom = %d", lense_ext->thermal_zoom);
	skyfend_log_debug("visable_focal = %d", lense_ext->visable_focal);
	skyfend_log_debug("thermal_focal = %d", lense_ext->thermal_focal);

	return 0;
}

static int32_t ptz_sys_ext_info_0x15(ptz_protocol_msg_t *msg)
{
	ptz_msg_sys_ext_info_t *sys_ext = (ptz_msg_sys_ext_info_t *)msg->msg; 

	if(NULL == sys_ext) {
		skyfend_log_info("%s:%d msg is null\n", __FUNCTION__, __LINE__);
		return 0;
	}

	skyfend_log_debug("photoelectricity_no = %d", sys_ext->photoelectricity_no);
	skyfend_log_debug("cur_status_timesmap = %lu", sys_ext->cur_status_timesmap);
	skyfend_log_debug("laser_rangefinder_status = %d", sys_ext->laser_rangefinder_status);
	skyfend_log_debug("laser_measuring_status = %d", sys_ext->laser_measuring_status);
	skyfend_log_debug("laser_fill_state = %d", sys_ext->laser_fill_state);
	skyfend_log_debug("heater_condition = %d", sys_ext->heater_condition);
	skyfend_log_debug("thermal_imaging_status = %d", sys_ext->thermal_imaging_status);
	skyfend_log_debug("visible_focusing_mode = %d", sys_ext->visible_focusing_mode);
	skyfend_log_debug("thermal_focusing_mode = %d", sys_ext->thermal_focusing_mode);
	skyfend_log_debug("power_status_ext_device = %x", sys_ext->power_status_ext_device);

	return 0;
}

static int register_control_ptz_comands(void)
{
	int ret = 0;

	ret = eth_ptz_command_register(PTZ_COMMAND_DEV_STATUS, ptz_dev_status);
	ret |= eth_ptz_command_register(PTZ_COMMAND_TARGET_INFO, ptz_target_info);
	ret |= eth_ptz_command_register(PTZ_COMMAND_ATT_MSG, ptz_pitch_azimuth);
	ret |= eth_ptz_command_register(PTZ_COMMAND_DEV_EXT_INFO, ptz_dev_ext_info_0x08);
	ret |= eth_ptz_command_register(PTZ_COMMAND_LENSE_EXT_INFO, ptz_lense_ext_info_0x0c);
	ret |= eth_ptz_command_register(PTZ_COMMAND_SYS_EXT_INFO, ptz_sys_ext_info_0x15);

	return ret;
}

static int ptz_thread_load(struct ptz_ctrl *pnc)
{
	int ret = 0;

	ret = pthread_create(&(pnc->ptz_tid), NULL, ptz_camera_stream_thread, pnc);
	if(0 != ret) {
		skyfend_log_error("ptz stream thread loaded fail, ret = %d\n", ret);
		return -1;
	}

	return 0;
}

int capture_ptz_vstream_pic(videostreaminfo_t *vsinfo, cv::Mat &frame)
{
	uchar3* image = NULL;
	int status = 0;
	int ret = -1;
	int loop_time = 10; // 2 * PTZ_CAPTURE_TIMEOUT_VALUE = x (ms)
	struct ptz_ctrl *ptz_p = &ptzobj;
	
	ret = pthread_mutex_lock(&(ptz_p->mutex));
	if(0 != ret) {
        skyfend_log_error("%s:%d> get lock fail = %d", __FUNCTION__, __LINE__, ret);
        return -1;
    }
	if(!ptz_p->q.empty()) {
		frame = ptz_p->q.front();
		ptz_p->q.pop();
		ret = 0;
    } else {
		ret = -1;
    }
    pthread_mutex_unlock(&(ptz_p->mutex));

	return ret;

	do {
		if (!ptz_p->stream.isOpened()) {
				skyfend_log_error("ptz stream not opened\n");
				break;
		}
		if(!ptz_p->stream.read(frame)) {
			std::cout << "get vedio frame fail" << std::endl;
				loop_time--;
				skyfend_msleep(200);
				if (!ptz_p->stream.isOpened()) {
						skyfend_log_error("get vedio frame fail, ptz stream not opened\n");
						ptz_p->stop_capture = true;
						break;
				}
		} else {
				vsinfo->width = ptz_p->stream.get(CAP_PROP_FRAME_WIDTH);
				vsinfo->height = ptz_p->stream.get(CAP_PROP_FRAME_HEIGHT);
				vsinfo->framecount = ptz_p->stream.get(CAP_PROP_POS_FRAMES);
				vsinfo->rtp_timestamp = ptz_p->stream.get(CAP_PROP_POS_MSEC);
				ret=0;
				break;
		}
	} while(loop_time > 0);

	return ret;
}

int load_ptz(void)
{
    int ret = -1;

	ret = eth_link_ptz_init();
	if(0 != ret) {
		skyfend_log_error("init ptz link fail, ret = %d\n", ret);
		return ret;
	}
	ret = register_control_ptz_comands();
	if(0 != ret) {
		skyfend_log_error("register ptz command fail, ret = %d\n", ret);
		return ret;
	}
	sprintf(ptzobj.rtsp_stream_addr, "%s", DEFAULT_RTSP_PTZ);
	ptzobj.stop_capture = false;
    ret = ptz_thread_load(&ptzobj);

    return ret;
}

static int ptz_comands_unregister(void)
{
	return eth_link_ptz_uninit();
}

int relase_ptz_resource(void)
{
	void *rval = NULL;

	(void)rval;
	ptzobj.stop_capture = true;
	ptzobj.stop_stream = true;
	pthread_join(ptzobj.ptz_tid, &rval);
	ptz_comands_unregister();

	return 0;
}
