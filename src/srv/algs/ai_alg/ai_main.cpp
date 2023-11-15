// Test.cpp : Defines the entry point for the DLL application.
//
#include <iostream>
#include <fstream>
#include <ctime>
// #include <json/json.h>
#include <thread>
#include <chrono>
#include <opencv2/opencv.hpp>
#include "config.h"
// #include "../sdk/tmTransDefine.h"
// #include "../sdk/tmControlClient.h"
#include <cmath>
// #include "yolov8.hpp"
#include "BYTETracker.h"
#include "STrack.h"
// #include "chrono"
#include "tf.h"
#include "guide.h"
#include "skyfend_log.h"
#include "ai_main.h"
#include "ptz_control.h"
#include "alink_target.h"


int g_nID = -1;

// double startLat = 22.597008; // 实际矫正
// double startLng = 113.998115;
// double res[6] = {0, 0, 0, 0, 0, 0}; // x,y,z,a,e,range

// calibPTZ(startLat, startLng, obj_to_ptz.droneLatitude, obj_to_ptz.droneLongitude, obj_to_ptz.droneHeight, res);

// double BS;
// GetBSFromDistance(res[5], BS);
// BS = BS * 1.5;

// std::cout << "tracer: " << obj_to_ptz.droneLatitude << "," << obj_to_ptz.droneLongitude << "," << obj_to_ptz.droneHeight << std::endl;
// std::cout << "hhhhhhhhhhhhhhhhhhhhh-azimuth-hhhhhhhhhhhhh: " << res[3] << std::endl;
// std::cout << "vvvvvvvvvvvvvvvvvvvvv-elevation-vvvvvvvvvvvvv: " << res[4] << std::endl;

// float p = res[3] + 123.48;
// float t = res[4] - 0.67;

const std::vector<std::string> CLASS_NAMES = {
	"uav",
	"bird",
};
const std::vector<std::vector<unsigned int>> COLORS = {
	{0, 114, 189},
	{217, 83, 25},
};

ptz_msg_pitch_azimuth_t realtime_pa_t = {0};
int32_t ptz_pitch_azimuth(ptz_protocol_msg_t *msg)
{
	ptz_msg_pitch_azimuth_t *pa_t = NULL;

	pa_t = (ptz_msg_pitch_azimuth_t *)msg->msg;
#if 0
	skyfend_log_debug("photoelectricity_no = %d", pa_t->photoelectricity_no);
	skyfend_log_debug("photoelectricity_status_timesmap = %lu", pa_t->photoelectricity_status_timesmap);
	skyfend_log_debug("horizatal_angle = %f", pa_t->horizatal_angle);
	skyfend_log_debug("pitching_angle = %f", pa_t->pitching_angle);
	skyfend_log_debug("distance = %f", pa_t->distance);
	skyfend_log_debug("horizatal_angle_speed = %f", pa_t->horizatal_angle_speed);
	skyfend_log_debug("pitch_angle_speed = %f", pa_t->pitch_angle_speed);
	skyfend_log_debug("target_height = %d", pa_t->target_height);
	skyfend_log_debug("lens_magnification = %d", pa_t->lens_magnification);
	skyfend_log_debug("reserver = %d", pa_t->reserver);
#endif

	memcpy(&realtime_pa_t, pa_t, sizeof(ptz_msg_pitch_azimuth_t));

	return 0;
}

//0x01
ptz_msg_status_t realtime_status = {0};
int32_t ptz_dev_status(ptz_protocol_msg_t *msg)
{
	skyfend_log_info("ptz_dev_status\n");
	skyfend_log_info("ptz_dev_status-----");
	skyfend_log_info("ptz dev status info(%x)=>len = %d", msg->command, msg->msglen);
	for(int i=0; i<msg->msglen; i++) {
		printf("%02x ", msg->msg[i]);
	}
	printf("\n");
	if(sizeof(ptz_msg_status_t) >= msg->msglen) {
		ptz_msg_status_t *status = (ptz_msg_status_t *)msg->msg;
		memcpy(&realtime_status, status, sizeof(ptz_msg_status_t));
		skyfend_log_debug("photoelectricity_no = %d", status->photoelectricity_no);
		skyfend_log_debug("photoelectricity_status_timesmap = %lu", status->photoelectricity_status_timesmap);
		skyfend_log_debug("work_status = %d", status->work_status);
		skyfend_log_debug("work_mode = %d", status->work_mode);
		skyfend_log_debug("bitcode = %lu", status->bitcode);
		skyfend_log_debug("reserver = %d", ((int32_t*)status->reserver)[0]);
		skyfend_log_debug("realtime_status = %d", realtime_status.work_mode);
	}

	return 0;
}

ptz_msg_dev_ext_info_t realtime_dev_ext={0};//光电设备状态扩展信息包,work_status:0空闲,1搜索,2跟踪, by cdx
ptz_msg_dev_status_search_t realtime_sserach={0};
ptz_msg_dev_target_info_t realtime_target={0};//光电设备工作状态（搜索）外带数据信息包,可能有多个目标
ptz_msg_dev_status_track_t realtime_strack={0};//光电设备工作状态（跟踪）外带数据信息包,1个目标
int32_t ptz_dev_ext_info_0x08(ptz_protocol_msg_t *msg)
{
	ptz_msg_dev_ext_info_t *dev_ext = (ptz_msg_dev_ext_info_t *)msg->msg; 
	memcpy(&realtime_dev_ext, dev_ext, sizeof(ptz_msg_dev_ext_info_t));

	if(NULL == dev_ext) {
		skyfend_log_info("%s:%d msg is null\n", __FUNCTION__, __LINE__);
		return 0;
	}
	skyfend_log_debug("photoelectricity_no = %d\n", dev_ext->photoelectricity_no);
	skyfend_log_debug("sys_timesmap = %lu\n", dev_ext->sys_timesmap);
	skyfend_log_debug("work_status = %d\n", dev_ext->work_status);
	skyfend_log_debug("data_len = %d\n", dev_ext->data_len);
	if(dev_ext->work_status == 0) {
		if(dev_ext->data_len >= sizeof(int)) {
			int *time = (int *)(msg->msg + sizeof(ptz_msg_dev_ext_info_t));
			skyfend_log_debug("time = %d", time[0]);
		}
	} else if(dev_ext->work_status == 1) {
		ptz_msg_dev_status_search_t *sserach = (ptz_msg_dev_status_search_t *)(msg->msg + sizeof(ptz_msg_dev_ext_info_t));
		memcpy(&realtime_sserach, sserach, sizeof(ptz_msg_dev_status_search_t));

		skyfend_log_debug("time = %d", sserach->time);
		skyfend_log_debug("target_num = %d", sserach->target_num);
		skyfend_log_debug("data_len = %d", sserach->data_len);

		if(sserach->data_len >= sizeof(ptz_msg_dev_target_info_t)) {
		int num =  sserach->data_len/sizeof(ptz_msg_dev_target_info_t);
		ptz_msg_dev_target_info_t *target = (ptz_msg_dev_target_info_t *)(msg->msg + sizeof(ptz_msg_dev_ext_info_t) + sizeof(ptz_msg_dev_status_search_t));
			for(int i=0; i<num; i++) {
				skyfend_log_debug("target %d ===> ", i);
				skyfend_log_debug("targt_no = %d", target[i].targt_no);
				skyfend_log_debug("target_type = %d", target[i].target_type);//目标类型ID
				skyfend_log_debug("similarity = %d", target[i].similarity);
				skyfend_log_debug("target_width = %d", target[i].target_width);
				skyfend_log_debug("target_height = %d", target[i].target_height);
				skyfend_log_debug("target_azimuth = %f", target[i].target_azimuth);
				skyfend_log_debug("target_pitching_angle = %f", target[i].target_pitching_angle);
			}
			memcpy(&realtime_target, target, sizeof(ptz_msg_dev_target_info_t));
		}		
	} else if(dev_ext->work_status == 2) {
		ptz_msg_dev_status_track_t *strack = (ptz_msg_dev_status_track_t *)(msg->msg + sizeof(ptz_msg_dev_ext_info_t));
		skyfend_log_debug("time = %d", strack->time);
		skyfend_log_debug("user_id = %d", strack->user_id);
		skyfend_log_debug("target_status = %d", strack->target_status);//是否丢失 1-锁定 0-丢失
				
		skyfend_log_debug("targt_no = %d", strack->target.targt_no);
		skyfend_log_debug("target_type = %d", strack->target.target_type);
		skyfend_log_debug("similarity = %d", strack->target.similarity);
		skyfend_log_debug("target_width = %d", strack->target.target_width);
		skyfend_log_debug("target_height = %d", strack->target.target_height);
		skyfend_log_debug("target_azimuth = %f", strack->target.target_azimuth);
		skyfend_log_debug("target_pitching_angle = %f", strack->target.target_pitching_angle);

		memcpy(&realtime_strack, strack, sizeof(ptz_msg_dev_status_track_t));
	} else {
		skyfend_log_debug("can't recongize this work status");
	}

	return 0;
}

//0x0B
ptz_msg_target_pac_t realtime_targets={0}; 
int32_t ptz_target_info(ptz_protocol_msg_t *msg)
{
	skyfend_log_debug("ptz targets info(%x)\n", msg->command);
	// for(int i=0; i<msg->msglen; i++) {
	// 	printf("%02x ", msg->msg[i]);
	// }
	// printf("\n");

	ptz_msg_target_pac_t *targets = (ptz_msg_target_pac_t *)msg->msg;
	memcpy(&realtime_targets, targets, sizeof(ptz_msg_target_pac_t));
	skyfend_log_debug("last_time  = %d\n", targets->last_time);
	skyfend_log_debug("target_num = %d\n", targets->target_num);
	skyfend_log_debug("data_len = %d\n", targets->data_len);
    ptz_msg_target_info_t *ltargets = NULL;

	if(targets->target_num > 0) {
		ltargets= (ptz_msg_target_info_t *)(msg->msg + 12);
		if(!ltargets) {
			skyfend_log_error("%s:%d> memory error...\n", __FUNCTION__, __LINE__);
			return -1;
		}
		for(int i=0; i<targets->target_num; i++) {
			skyfend_log_debug("target %d info=>", i);
			skyfend_log_debug("target_no = %d", ltargets[i].target_no);
			skyfend_log_debug("target_type = %d", ltargets[i].target_type);
			skyfend_log_debug("similarty = %d", ltargets[i].similarty);
			skyfend_log_debug("width = %d", ltargets[i].width);
			skyfend_log_debug("height = %d", ltargets[i].height);
			skyfend_log_debug("phy_width = %d", ltargets[i].phy_width);
			skyfend_log_debug("phy_height = %d", ltargets[i].phy_height);
			skyfend_log_debug("motion_drirection = %d", ltargets[i].motion_drirection);
			skyfend_log_debug("ai_mode = %d", ltargets[i].ai_mode);
			skyfend_log_debug("reserver = %d", ltargets[i].reserver);
			skyfend_log_debug("horizatal_speed = %d", ltargets[i].horizatal_speed);
			skyfend_log_debug("vertical_speed = %d", ltargets[i].vertical_speed);
			skyfend_log_debug("target_position_x = %d", ltargets[i].target_position_x);
			skyfend_log_debug("target_position_y = %d", ltargets[i].target_position_y);
			skyfend_log_debug("target_azimuth = %lu", ltargets[i].target_azimuth);
			skyfend_log_debug("target_pitching = %lu", ltargets[i].target_pitching);
			skyfend_log_debug("target_distance = %d", ltargets[i].target_distance);
			skyfend_log_debug("\n");
		}
	}

	return 0;
}

int getPTZ(float *ptz)
{

	ptz[0] = realtime_pa_t.horizatal_angle;
	ptz[1] = realtime_pa_t.pitching_angle;
	ptz[2] = realtime_pa_t.lens_magnification;
	std::cout << "ptz:" << ptz[0] << "," << ptz[1] << "," << ptz[2] << std::endl;

	return 0;
}

void setPTZ(float ablHor, float ablVer,
			float cameraBS = 1.0, int m_iHorPtzSpeed = 40, int m_iVerPtzSpeed = 40)
{
	ptz_msgid_0x03_addr_t addr_t = {0};
	ptz_msg_ctrl_pack_t ctrl_p = {0};

	(void)m_iHorPtzSpeed;
	(void)m_iVerPtzSpeed;

	addr_t.guidance_mode = PTZ_ADDR_GUIDANCE_MODE_ANGLE;
	addr_t.horizatal_angle = ablHor;
	addr_t.pitch_angle = ablVer;
	addr_t.target_motion_direction = 0;
	// addr_t.target_altitude = 200;
	// addr_t.distance = 300;

	send_ptz_control_addr(&addr_t);

	ctrl_p.ctrl_cmd = 0x02;
	ctrl_p.focal = cameraBS;
	send_ptz_control_pack(&ctrl_p);
}

void setPTZ0(float ablHor, float ablVer,
			 float verticalHeight, float horizontalDistance,
			 float cameraBS = 1.0, int m_iHorPtzSpeed = 254, int m_iVerPtzSpeed = 254)
{
	ptz_msgid_0x03_addr_t addr_t = {0};
	ptz_msg_ctrl_pack_t ctrl_p = {0};

	(void)m_iHorPtzSpeed;
	(void)m_iVerPtzSpeed;

	addr_t.guidance_mode = PTZ_ADDR_GUIDANCE_MODE_ANGLE;
	addr_t.horizatal_angle = ablHor;
	addr_t.pitch_angle = ablVer;
	addr_t.target_motion_direction = 0;
	addr_t.target_altitude = verticalHeight;
	addr_t.distance = horizontalDistance;

	send_ptz_control_addr(&addr_t);

	// ctrl_p.ctrl_cmd = 0x02;
	// ctrl_p.focal = cameraBS;
	// send_ptz_control_pack(&ctrl_p);
}

//经纬度
void setPTZ1(float longitude, float latitude,
			float altitude, float horizontalDistance,
			float cameraBS = 1.0, int m_iHorPtzSpeed = 254, int m_iVerPtzSpeed = 254)
{
	ptz_msgid_0x03_addr_t addr_t = {0};
	ptz_msg_ctrl_pack_t ctrl_p = {0};
	
	(void)m_iHorPtzSpeed;
	(void)m_iVerPtzSpeed;

	addr_t.guidance_mode = 1;//PTZ_ADDR_GUIDANCE_MODE_ANGLE;
	addr_t.target_longitude = longitude;
	addr_t.target_latitude = latitude;
	addr_t.target_altitude = altitude;
	addr_t.target_motion_direction = 0;
	addr_t.distance = horizontalDistance;

	send_ptz_control_addr(&addr_t);
	
	// ctrl_p.ctrl_cmd = 0x02;
	// ctrl_p.focal = cameraBS;
	// send_ptz_control_pack(&ctrl_p);
}


void SetCameraZoom(float cameraBS)
{
	ptz_msg_ctrl_pack_t ctrl_p = {0};

	ctrl_p.ctrl_cmd = 0x02;
	ctrl_p.focal = cameraBS;
	send_ptz_control_pack(&ctrl_p);
}

void StopTraceAutoZoom()
{
}

void PTZTracking(int left, int right, int top, int bottom)
{
	ptz_msg_manual_lock_target_t track = {0};

	/*该坐标必须映射到704X576 的坐标系里，这里假设客户端视频显示的区域大小为640X480*/
	float fRationX = (float)704 / 640;
	float fRationY = (float)576 / 480;

	track.track_vedio_source = 0;
	track.targegt_cor_y = ((top + bottom) / 2) * fRationY;
	track.target_cor_x = ((left + right) / 2) * fRationX;
	send_ptz_control_manual_lock_target(&track);
}

void StoppingTracking()
{
	ptz_msg_track_t track = {0};

	track.ctrl_cmd = 3;
	send_ptz_control_track(&track);
}

void StartScanRegion(uint32_t horizatal_begin, uint32_t horizatal_end,
					 uint32_t pitching_begin, uint32_t pitching_end, uint32_t scan_speed = 80)
{
	ptz_msg_scan_ext_t scan = {0};

	scan.ctrl_cmd = 1; // 启动扫描
	scan.horizatal_search_begin = horizatal_begin;
	scan.horizatal_search_end = horizatal_end;
	scan.pitching_search_begin = pitching_begin;
	scan.pitching_search_end = pitching_end;
	scan.scan_speed = scan_speed; // 度/秒

	send_ptz_control_scan_ext(&scan);
}

void StopScanRegion()
{
	ptz_msg_scan_ext_t scan = {0};

	scan.ctrl_cmd = 0; // 停止扫描

	send_ptz_control_scan_ext(&scan);
}

void StartSDT(uint32_t horizatal_begin, uint32_t horizatal_end,
					 uint32_t pitching_begin, uint32_t pitching_end, uint32_t scan_speed = 80)
{
	ptz_msg_track_t sdt = {0};
    sdt.ctrl_cmd = 1;//4
    // sdt.horizatal_search_begin=horizatal_begin;
    // sdt.horizatal_search_end=horizatal_end;
    // sdt.pitching_search_begin=pitching_begin;
    // sdt.pitching_search_end=pitching_end;
	sdt.reserver=0;
	send_ptz_control_track(&sdt);
	std::cout << "------------------StartSDT"  << std::endl;
}

void StopSDT()
{
	ptz_msg_track_t sdt = {0};
	sdt.ctrl_cmd = 3;
	send_ptz_control_track(&sdt);
}

void GetBSFromDistance(double distance, double &BS)
{
	double dRatio = 0.6;
	if (distance < 100)
	{
		BS = 14.0 * dRatio;
	}
	else if (distance < 200 && distance >= 100)
	{
		BS = 17.0 * dRatio;
	}
	else if (distance < 300 && distance >= 200)
	{
		BS = 24.0 * dRatio;
	}
	else if (distance < 400 && distance >= 300)
	{
		BS = 34.0 * dRatio;
	}
	else if (distance < 500 && distance >= 400)
	{
		BS = 40.0 * dRatio;
	}
	else if (distance < 600 && distance >= 500)
	{
		BS = 44.0 * dRatio;
	}
	else if (distance < 800 && distance >= 600)
	{
		BS = 46.0 * dRatio;
	}
	else
	{
		BS = 47.0 * dRatio;
	}
}

void upload_target_info(int boxNum,
						int *classIndexs,
						int *boxes,
						float *conf,
						uint32_t timestamp)
{
	if (boxNum <= 0)
	{
		return;
	}

	alink_taget_info_t tagetInfo;
	tagetInfo.timestamp = timestamp;
	tagetInfo.objNum = boxNum;
	for (uint16_t i = 0; boxNum > i; ++i)
	{
		tagetInfo.targets[i].id = 1;
		tagetInfo.targets[i].classification = classIndexs[i];
		tagetInfo.targets[i].rectX = boxes[4 * i];
		tagetInfo.targets[i].rectY = boxes[4 * i + 1];
		tagetInfo.targets[i].rectW = boxes[4 * i + 2];
		tagetInfo.targets[i].rectH = boxes[4 * i + 3];
		tagetInfo.targets[i].rectXV = 0x77;
		tagetInfo.targets[i].rectYV = 0x88;
		tagetInfo.targets[i].classfyProb = conf[i] * 100;
		// psTarget->targets[i].reserve
	}
	alink_upload_target_info(&tagetInfo);
}

void test_upload_target_info(void)
{
	int BboxNum = 3;
	int ClassIndexs[10] = {1, 2, 3};
	int Boxes[40] = {0};
	float Conf[10] = {0.4, 0.8, 0.9};

	for (uint8_t i = 0; i < BboxNum; ++i)
	{
		Boxes[3 * i] = i * 10;
		Boxes[3 * i + 1] = i * 10 + 1;
		Boxes[3 * i + 2] = i * 10 + 2;
		Boxes[3 * i + 3] = i * 10 + 3;
	}

	upload_target_info(BboxNum, ClassIndexs, Boxes, Conf, 0);
}

void YOLOV8Infer(
	const std::string engine_path,
	const std::string img_path)
{
	// cuda:0
	cudaSetDevice(0);

	auto yolov8 = new YOLOv8(engine_path);
	auto tracker = new BYTETracker(15, 15);
	yolov8->make_pipe(true);

	cv::Mat res, image;
	cv::Size size = cv::Size{1280, 1280};
	std::vector<Object> objs;

	image = cv::imread(img_path);
	yolov8->copy_from_Mat(image, size);
	auto start = std::chrono::system_clock::now();
	yolov8->infer();
	auto end = std::chrono::system_clock::now();
	yolov8->postprocess(objs);
	auto trackerstart = std::chrono::system_clock::now();
	vector<STrack> output_stracks = tracker->update(objs);
	auto trackerend = std::chrono::system_clock::now();
	for (int i = 0; i < output_stracks.size(); i++)
	{
		vector<float> tlwh = output_stracks[i].tlwh;
		// bool vertical = tlwh[2] / tlwh[3] > 1.6;
		// if (tlwh[2] * tlwh[3] > 20 && !vertical)
		if (tlwh[2] * tlwh[3] > 20)
		{
			std::cout << " ID:  " << output_stracks[i].track_id << std::endl;
			yolov8->draw_objects(image, res, objs, CLASS_NAMES, COLORS, output_stracks[i].track_id);
		}
	}
	// yolov8->draw_objects(image, res, objs, CLASS_NAMES, COLORS, 0);
	auto tc = (double)
				  std::chrono::duration_cast<std::chrono::microseconds>(end - start)
					  .count() /
			  1000.;
	printf("Detection cost %2.4lf ms\n", tc);
	auto tt = (double)
				  std::chrono::duration_cast<std::chrono::microseconds>(trackerend - trackerstart)
					  .count() /
			  1000.;
	printf("Tracker cost %2.4lf ms\n", tt);

	cv::resize(res, res, cv::Size(960, 540));
	cv::imshow("result", res);
	// cv::imwrite("result1.png",res);
	cv::waitKey(0);
	delete yolov8;
}

std::vector<std::pair<float, float>> ScanRegionPoints(int region_size = 30, int step_gap = 5, std::pair<float, float> start_point = {10, 20})
{
	int steps = region_size / step_gap;
	std::vector<std::pair<float, float>> points;
	for (int i = 0; i < steps + 1; ++i)
	{
		float x = start_point.first + i * step_gap;
		float y1 = start_point.second;
		float y2 = start_point.second + region_size;
		if (i % 2 == 0)
		{
			points.push_back({x, y1});
			points.push_back({x, y2});
		}
		else
		{
			points.push_back({x, y2});
			points.push_back({x, y1});
		}
	}
	return points;
}

int scan_region(float fH = 52.8, float fV = 17.6)
{
	float fOffH = 30.0;
	float fOffV = 30.0;
	float z = 7.9;
	setPTZ(fH, fV, z);
	usleep(4000 * 1000);

	// 左下-->左上
	//  float a=fH-fOffH/2;
	//  float e=fV-fOffV/2;
	float a = fH - 1.5;
	float e = fV - 0; // 0~28

	float a_step = 3.0;
	setPTZ(a, e, z);
	usleep(3000 * 1000);
	// float ptz[3]={0,0,0};
	// getPTZ(hTmcc,ptz);
	// return 0;

	double time1 = static_cast<double>(cv::getTickCount());
	for (int i = 0; i < 10; i++)
	{
		std::cout << "i:" << i << "===========a:" << a << ",e:" << e << ",z:" << z << std::endl;
		if (i % 2 == 0)
			e = fV + fOffV / 2;
		else
			e = fV - fOffV / 2;
		setPTZ(a, e, z); // V方向,偶向上奇向下
		usleep(850 * 1000);
		std::cout << "a:" << a << ",e:" << e << ",z:" << z << std::endl;
		setPTZ(a + a_step, e, z); // H方向,向角增大的方向(顺时针)
		usleep(400 * 10000);
		a = a + a_step;
	}
	double time2 = static_cast<double>(cv::getTickCount());
	double time_use = (time2 - time1) / cv::getTickFrequency();
	std::cout << "Time use: " << time_use << "s" << std::endl; // 输出运行时间
}

void radarGuide(int img_count, radar_track_info_packages_t *radar_targets, int ptz_id = -1)
{
	std::vector<RadarObj> obj_list;
	int ret = radar_read(radar_targets, obj_list);
	if (ret < 0)
		return;

	RadarObj obj_to_ptz;
	ret = radar_2ptz(&obj_list, obj_to_ptz, ptz_id);

	// std::cout << " +++++++ obj to ptz: " << ret << std::endl;

	// if(ret > 0 && img_count%10 == 0)
	// if(ret > 0 && obj_to_ptz.id == ptz_id)  //cdx
	if (ret > 0)
	{
		StoppingTracking();

		double BS;
		GetBSFromDistance(obj_to_ptz.range, BS);

		// double horAngle = 360 - obj_to_ptz.azimuth;
		std::cout << "hhhhhhhhhhhhhhhhhhhhh-azimuth-hhhhhhhhhhhhh: " << obj_to_ptz.azimuth << std::endl;
		std::cout << "vvvvvvvvvvvvvvvvvvvvv-elevation-vvvvvvvvvvvvv: " << obj_to_ptz.elevation << std::endl;
		// std::cout << "hhhhhhhhhhhhhhhhhhhhh-azimuth-ptz-hhhhhhhhhhhhh: " << horAngle << std::endl;
		// std::cout << "vvvvvvvvvvvvvvvvvvvvv-elevation-ptz-vvvvvvvvvvvvv: " << obj_to_ptz.elevation << std::endl;
		// std::cout << "gNID-ptz_id-id-xyz: " << g_nID <<"," << ptz_id <<"," << obj_to_ptz.id << "," << obj_to_ptz.x << "," <<obj_to_ptz.y <<  "," <<obj_to_ptz.z << std::endl;


		// // PTZTurnTargetAblAngle(m_hControl, horAngle, obj_to_ptz.elevation+90, BS);
		// // setPTZ(horAngle, obj_to_ptz.elevation + 90, BS);
		// // setPTZ0(120, 20, 100, 200);
		// setPTZ0(horAngle, obj_to_ptz.elevation, sqrt(obj_to_ptz.x*obj_to_ptz.x+ obj_to_ptz.y*obj_to_ptz.y), obj_to_ptz.z);
		// // setPTZ0( obj_to_ptz.azimuth, obj_to_ptz.elevation, 200, obj_to_ptz.z);

		double horAngle =0;
		if(obj_to_ptz.azimuth < 0)
			horAngle = 360 + obj_to_ptz.azimuth;
		setPTZ0(horAngle, obj_to_ptz.elevation, sqrt(obj_to_ptz.x*obj_to_ptz.x+ obj_to_ptz.y*obj_to_ptz.y), obj_to_ptz.z);
		sleep(5);
	}
}

void tracerGuideR(int img_count, remoteid_track_info_packages_t *tracer_targets)
{
	std::vector<TracerObj> obj_list;
	int ret = -1;
	ret = tracerR_read(tracer_targets, obj_list);

	if (ret < 0)
		return;

	TracerObj obj_to_ptz;
	ret = tracer_2ptz(&obj_list, obj_to_ptz);

	// std::cout << " +++++++ obj to ptz: " << ret << std::endl;
	// if(ret > 0 && img_count % 60 == 0)
	// if(ret > 0 && obj_to_ptz.id == ptz_id)  //cdx
	if (ret > 0)
	{
		// StoppingTracking();

		// 		double g_startLat =  22.596956;//yu3 22.596903;//22.596950;//22.596892600;// 22.5968793; //22.596875;  //实际矫正  需初始设置
		// double g_startLng = 113.998123;//yu3 113.998091 ;//113.998075 ;//113.998086638;//113.99808875;//113.998116;
		// double g_p_OFF = 122.63;
		// double g_t_OFF = 0.25;

		double startLat = 22.596960; // 实际矫正
		double startLng = 113.998050;
		double res[6] = {0, 0, 0, 0, 0, 0}; // x,y,z,a,e,range

		calibPTZ(startLat, startLng, obj_to_ptz.droneLatitude, obj_to_ptz.droneLongitude, obj_to_ptz.droneHeight, res);

		// double BS;
		// GetBSFromDistance(res[5], BS);//cdx
		// BS = BS * 1.5;
		// BS = 5;

		std::cout << "hhhhhhhhhhhhhhhhhhhhh-azimuth-hhhhhhhhhhhhh: " << res[3] << std::endl;
		std::cout << "vvvvvvvvvvvvvvvvvvvvv-elevation-vvvvvvvvvvvvv: " << res[4] << std::endl;

		float p = res[3] + 97.86;
		float t = res[4] - -0.16;
		std::cout << "hhhhhhhhhhhhhhhhhhhhh-azimuth-ptz-hhhhhhhhhhhhh: " << p << std::endl;
		std::cout << "vvvvvvvvvvvvvvvvvvvvv-elevation-ptz-vvvvvvvvvvvvv: " << t << std::endl;

		// p=100;
		// t=30;
		// BS=2;
		// PTZTurnTargetAblAngle(m_hControl, horAngle, obj_to_ptz.elevation+90, BS);
		setPTZ0(p, t, obj_to_ptz.droneHeight, res[5]); // 坐标系,耐杰下0,中90,上180；左负或290,右正

		// cv::waitKey(1500);//cdx
	}
}

void tracerGuideD(int img_count, droneid_track_info_packages_t *tracer_targets)
{
	std::vector<TracerObj> obj_list;
	int ret = -1;
	ret = tracerD_read(tracer_targets, obj_list);
	if (ret < 0)
		return;

	TracerObj obj_to_ptz;
	ret = tracer_2ptz(&obj_list, obj_to_ptz);

	// std::cout << " +++++++ obj to ptz: " << ret << std::endl;
	// if(ret > 0 && img_count % 60 == 0)
	// if(ret > 0 && obj_to_ptz.id == ptz_id)  //cdx
	if (ret > 0)
	{
		// StoppingTracking();

		double startLat = 22.596960; // 实际矫正
		double startLng = 113.998050;
		double res[6] = {0, 0, 0, 0, 0, 0}; // x,y,z,a,e,range

		calibPTZ(startLat, startLng, obj_to_ptz.droneLatitude, obj_to_ptz.droneLongitude, obj_to_ptz.droneHeight, res);

		double BS;
		GetBSFromDistance(res[5], BS);
		BS = BS * 1.5;

		std::cout << "tracer: " << obj_to_ptz.droneLatitude << "," << obj_to_ptz.droneLongitude << "," << obj_to_ptz.droneHeight << std::endl;
		std::cout << "hhhhhhhhhhhhhhhhhhhhh-azimuth-hhhhhhhhhhhhh: " << res[3] << std::endl;
		std::cout << "vvvvvvvvvvvvvvvvvvvvv-elevation-vvvvvvvvvvvvv: " << res[4] << std::endl;

		float p = res[3] + 97.86;
		float t = res[4] - 0.16;
		std::cout << "hhhhhhhhhhhhhhhhhhhhh-azimuth-ptz-hhhhhhhhhhhhh: " << p << std::endl;
		std::cout << "vvvvvvvvvvvvvvvvvvvvv-elevation-ptz-vvvvvvvvvvvvv: " << t << std::endl;

		setPTZ0(p, t, obj_to_ptz.droneHeight, res[5]);
		// static int flag = 0;
		// if(flag == 0) {
		// 	StartSDT(100, 150, 0, 10);
		// 	flag = 1;
		// }
	}
}

void tracerGuideD_N(int img_count, droneid_track_info_packages_t *tracer_targets)
{

	std::vector<TracerObj> obj_list;
	int ret = -1;
	ret = tracerD_read(tracer_targets, obj_list);
	if (ret < 0)
		return;

	TracerObj obj_to_ptz;
	ret = tracer_2ptz(&obj_list, obj_to_ptz);

	// std::cout << " +++++++ obj to ptz: " << ret << std::endl;
	// if(ret > 0 && img_count % 60 == 0)
	// if(ret > 0 && obj_to_ptz.id == ptz_id)  //cdx
	if (ret > 0)
	{
		StoppingTracking();

		double startLat = 22.596927; // 实际矫正
		double startLng = 113.998072;
		double res[6] = {0, 0, 0, 0, 0, 0}; // x,y,z,a,e,range

		calibPTZ(startLat, startLng, obj_to_ptz.droneLatitude, obj_to_ptz.droneLongitude, obj_to_ptz.droneHeight, res);

		double BS;
		GetBSFromDistance(res[5], BS);
		BS = BS * 1.5;

		std::cout << "tracer: " << obj_to_ptz.droneLatitude << "," << obj_to_ptz.droneLongitude << "," << obj_to_ptz.droneHeight << std::endl;
		std::cout << "hhhhhhhhhhhhhhhhhhhhh-azimuth-hhhhhhhhhhhhh: " << res[3] << std::endl;
		std::cout << "vvvvvvvvvvvvvvvvvvvvv-elevation-vvvvvvvvvvvvv: " << res[4] << std::endl;

		float p = res[3] + 126.83;
		float t = res[4] - 0.64;
		std::cout << "hhhhhhhhhhhhhhhhhhhhh-azimuth-ptz-hhhhhhhhhhhhh: " << p << std::endl;
		std::cout << "vvvvvvvvvvvvvvvvvvvvv-elevation-ptz-vvvvvvvvvvvvv: " << t << std::endl;

		setPTZ0(p, t, obj_to_ptz.droneHeight, res[5]);
		// setPTZ1(obj_to_ptz.droneLongitude, obj_to_ptz.droneLatitude, obj_to_ptz.droneHeight, res[5]);
		// sleep(1);

		// static int flag = 0;
		// if(flag == 0) {
		// 	StartSDT(100, 150, 0, 10);
		// 	flag = 1;
		// }

		// StartSDT(10000, 15000, 0, 8000);
		// StartSDT(100, 300, 0, 80);
		// StartSDT(p, t, obj_to_ptz.droneHeight, res[5]);
		// static int flag = 0;
		// StartSDT((p-20)*100, (p+20)*100, (t-10)*100, (t+10)*100);
		// if (0==flag){
		// 	StartSDT((p-20)*100, (p+20)*100, (t-10)*100, (t+10)*100);
		// 	std::cout << "------------------flag:"  <<flag << std::endl;
		// 	flag++;
		// }
	}
}


int ai_alg_run0(cv::Mat &image, void *targets)
{
	const std::string engine_file_path = "/home/cdx/6learn/ptz/ptz-nj/models/yolov8s_fp16_1280_0915.engine";
	radar_track_info_packages_t *radar_t = NULL;
	remoteid_track_info_packages_t *remoteid = NULL;
	droneid_track_info_packages_t *droneid = NULL;
	track_info_packages_t *track_info = (track_info_packages_t *)targets;
	int data_dev_type = 0;

	if (!targets)
	{
		return -1;
	}

	data_dev_type = track_info->track_pack_type;
	switch (data_dev_type)
	{
	case TRACK_INFO_RADAR:
		radar_t = (radar_track_info_packages_t *)&(track_info->track_info[0]);
		break;
	case TRACK_INFO_TRACER_P_REMOTE_ID:
		remoteid = (remoteid_track_info_packages_t *)&(track_info->track_info[0]);
		break;
	case TRACK_INFO_TRACER_P_DRONE_ID:
		droneid = (droneid_track_info_packages_t *)&(track_info->track_info[0]);
		break;
	default:
		return -1;
	}

	// GuideTracking(image, engine_file_path, true, radar_t);//true:radar;false:tracer

	return 0;
}

int ai_alg_run(cv::Mat &image, void *targets)
{
	const std::string engine_file_path = "/home/cdx/6learn/ptz/ptz-nj/models/yolov8s_fp16_1280_0915.engine";
	radar_track_info_packages_t *radar_t = NULL;
	remoteid_track_info_packages_t *remoteid = NULL;
	droneid_track_info_packages_t *droneid = NULL;
	user_track_target_packages_t *user_target = NULL;
	track_info_packages_t *track_info = (track_info_packages_t *)targets;
	int data_dev_type = 0;

	if (!targets)
	{
		return -1;
	}

	data_dev_type = track_info->track_pack_type;
	switch (data_dev_type)
	{
	case TRACK_INFO_RADAR:
		radar_t = (radar_track_info_packages_t *)targets;
		break;
	case TRACK_INFO_TRACER_P_REMOTE_ID:
		remoteid = (remoteid_track_info_packages_t *)targets;
		break;
	case TRACK_INFO_TRACER_P_DRONE_ID:
		droneid = (droneid_track_info_packages_t *)targets;
		break;
	case TRACK_INFO_FROM_USER:
		user_target = (user_track_target_packages_t *)targets;
	default:
		return -1;
	}

	// GuideTracking(image, engine_file_path, true, radar_t); // true:radar;false:tracer

	return 0;
}

int img_count = 0;
int no_obj_count = 0;
bool g_bInit = false;
auto yolov8 = new YOLOv8("/home/skyfend/workspace/ptz100_agx/models/uav.engine");
auto tracker = new BYTETracker(30, 30);
cv::Size size = cv::Size{1280, 1280};
bool isGuide = false;
int ai_alg_run1(cv::Mat &img, void *targets)
{
	radar_track_info_packages_t *radar_t = NULL;
	remoteid_track_info_packages_t *remoteid = NULL;
	droneid_track_info_packages_t *droneid = NULL;
	user_track_target_packages_t *user_target = NULL;
	track_info_packages_t *track_info = (track_info_packages_t *)targets; //(track_info_pac// tracerGuide(img_count, droneid);kages_t *)targets;
	int data_dev_type = 0;

	skyfend_log_info("%s:%d > g_bInit: %d", __FUNCTION__, __LINE__, g_bInit);

	if (!g_bInit)
	{
		yolov8->make_pipe(true);
		g_bInit = true;

		std::cout << "------------ 初次开始引导 -----------------------------" << data_dev_type << std::endl;
		if (targets)
		{
			data_dev_type = track_info->track_pack_type;
			switch (data_dev_type)
			{
			case TRACK_INFO_RADAR:
			{
				radar_t = (radar_track_info_packages_t *)targets;
				radarGuide(0, radar_t);
				break;
			}
			case TRACK_INFO_TRACER_P_REMOTE_ID:
			{
				remoteid = (remoteid_track_info_packages_t *)targets;
				tracerGuideR(0, remoteid);
				break;
			}
			case TRACK_INFO_TRACER_P_DRONE_ID:
			{
				droneid = (droneid_track_info_packages_t *)targets;
				tracerGuideD(0, droneid);
			}
			case TRACK_INFO_FROM_USER:
			{
				user_target = (user_track_target_packages_t *)targets;
				break;
			}
			}
		}
	}

	int img_h = img.rows;
	int img_w = img.cols;

	cv::Size size = cv::Size{1280, 1280};
	yolov8->copy_from_Mat(img, size);
	yolov8->infer();
	std::vector<Object> objs;
	yolov8->postprocess(objs);
	std::vector<STrack> output_stracks = tracker->update(objs);

	if (0 == output_stracks.size())
		no_obj_count++;
	else
		no_obj_count = 0;

	skyfend_log_info("11111111111111111111111111111111111111111111111111111111111111111 no_obj_count = %d", no_obj_count);
	if (no_obj_count > 16)
	{
		std::cout << "++++++++++++++++++++++++ 控制过程中引导 ++++++++++++++++++++++++++++" << std::endl;
		no_obj_count = 0;
		isGuide = true;
		if (targets && isGuide)
		{
			data_dev_type = track_info->track_pack_type;
			std::cout << "333333333333333333333333333333333333333333333333333333333:   " << data_dev_type << std::endl;
			switch (data_dev_type)
			{
			case TRACK_INFO_RADAR:
			{
				radar_t = (radar_track_info_packages_t *)targets;
				radarGuide(0, radar_t);
				break;
			}
			case TRACK_INFO_TRACER_P_REMOTE_ID:
			{
				remoteid = (remoteid_track_info_packages_t *)targets;
				tracerGuideR(0, remoteid);
				break;
			}
			case TRACK_INFO_TRACER_P_DRONE_ID:
			{
				droneid = (droneid_track_info_packages_t *)targets;
				tracerGuideD(0, droneid);
			}
			case TRACK_INFO_FROM_USER:
			{
				user_target = (user_track_target_packages_t *)targets;
				break;
			}
			}
		}
	}

	for (int i = 0; i < output_stracks.size(); i++)
	{
		std::vector<float> tlwh = output_stracks[i].tlwh;

		if (tlwh[2] * tlwh[3] > 4)
		{
			// std::cout << " ID:  " << output_stracks[i].track_id << std::endl;
			// yolov8->draw_objects(img, objs, CLASS_NAMES, COLORS, output_stracks[i].track_id);
			float score = objs[i].prob;
			int label_id = objs[i].label;
			if (score >= 0.3 && label_id == 0)
			{
				std::cout << ("AI找到了无人机") << std::endl;
			}

			bool frameGuide = (img_count % 16 == 0) && (objs[i].label == 0);
			if (frameGuide)
			{

				StoppingTracking();

				double scale_w = 640.0 / img_w;
				double scale_h = 480.0 / img_h;
				int x1 = tlwh[0] * scale_w;
				int x2 = (tlwh[0] + tlwh[2]) * scale_w;
				int y1 = tlwh[1] * scale_h;
				int y2 = (tlwh[1] + tlwh[3]) * scale_h;

				PTZTracking(x1, x2, y1, y2);
			}
			if (isGuide == true && frameGuide == false)
			{
				StoppingTracking();

				double scale_w = 640.0 / img_w;
				double scale_h = 480.0 / img_h;
				int x1 = tlwh[0] * scale_w;
				int x2 = (tlwh[0] + tlwh[2]) * scale_w;
				int y1 = tlwh[1] * scale_h;
				int y2 = (tlwh[1] + tlwh[3]) * scale_h;

				PTZTracking(x1, x2, y1, y2);
				isGuide = false;
			}
		}
	}
	img_count++;

	// bool bShow=true;
	// if (bShow){
	// 	cv::resize(img, img, cv::Size(800, 640));
	// 	cv::imshow("frame", img);
	// }
	// int keyPressed = cv::waitKey(1);
	// if (keyPressed == 'q') // Exit loop if ESC key is pressed
	// continueCapturing = false;

	return 0;
}

// int g_nID = -1;//放开头
int ai_alg_run2(cv::Mat &img, void *targets)
{
	radar_track_info_packages_t *radar_t = NULL;
	remoteid_track_info_packages_t *remoteid = NULL;
	droneid_track_info_packages_t *droneid = NULL;
	user_track_target_packages_t *user_target = NULL;
	track_info_packages_t *track_info = (track_info_packages_t *)targets; //(track_info_pac// tracerGuide(img_count, droneid);kages_t *)targets;
	int data_dev_type = 0;

	skyfend_log_info("%s:%d > g_bInit: %d", __FUNCTION__, __LINE__, g_bInit);

	if (!g_bInit)
	{
		yolov8->make_pipe(true);
		g_bInit = true;

		std::cout << "------------ 初次开始引导 -----------------------------" << data_dev_type << std::endl;
		if (targets)
		{
			data_dev_type = track_info->track_pack_type;
			switch (data_dev_type)
			{
			case TRACK_INFO_RADAR:
			{
				radar_t = (radar_track_info_packages_t *)targets;
				radarGuide(0, radar_t, g_nID);
				break;
			}
			case TRACK_INFO_TRACER_P_REMOTE_ID:
			{
				remoteid = (remoteid_track_info_packages_t *)targets;
				tracerGuideR(0, remoteid);
				break;
			}
			case TRACK_INFO_TRACER_P_DRONE_ID:
			{
				droneid = (droneid_track_info_packages_t *)targets;
				tracerGuideD(0, droneid);
				break;
			}
			case TRACK_INFO_FROM_USER:
			{
				user_target = (user_track_target_packages_t *)targets;
				g_nID = user_target->utt.objid;
				std::cout << "------------------1111 000000000000000000000000 c2 g_nID =" << g_nID << std::endl;
				StoppingTracking();
				return 1;
			}
			}
		}
	}
	else
	{
		if (targets)
		{
			data_dev_type = track_info->track_pack_type;
			switch (data_dev_type)
			{
			case TRACK_INFO_FROM_USER:
			{
				user_target = (user_track_target_packages_t *)targets;
				g_nID = user_target->utt.objid;
				std::cout << "------------------1122 000000000000000000000000 c2 g_nID =" << g_nID << std::endl;
				StoppingTracking();
				break;
			}
			}
		}
	}

	int img_h = img.rows;
	int img_w = img.cols;

	cv::Size size = cv::Size{1280, 1280};
	yolov8->copy_from_Mat(img, size);
	yolov8->infer();
	std::vector<Object> objs;
	yolov8->postprocess(objs);
	std::vector<STrack> output_stracks = tracker->update(objs);

	if (0 == output_stracks.size())
		no_obj_count++;
	else
		no_obj_count = 0;

	std::cout << "11111111111111111111111111111111111111111111111111111111111111111 no_obj_count =" << no_obj_count << std::endl;
	skyfend_log_info("11111111111111111111111111111111111111111111111111111111111111111 no_obj_count = %d", no_obj_count);
	// if (no_obj_count > 16 || g_nID > -1)
	if (no_obj_count > 16)
	{
		std::cout << "++++++++++++++++++++++++ 控制过程中引导 ++++++++++++++++++++++++++++" << std::endl;
		no_obj_count = 0;
		isGuide = true;
		if (targets && isGuide)
		{
			data_dev_type = track_info->track_pack_type;
			std::cout << "333333333333333333333333333333333333333333333333333333333:   " << data_dev_type << std::endl;
			switch (data_dev_type)
			{
			case TRACK_INFO_RADAR:
			{
				radar_t = (radar_track_info_packages_t *)targets;
				radarGuide(0, radar_t, g_nID);
				break;
			}
			case TRACK_INFO_TRACER_P_REMOTE_ID:
			{
				remoteid = (remoteid_track_info_packages_t *)targets;
				tracerGuideR(0, remoteid);
				break;
			}
			case TRACK_INFO_TRACER_P_DRONE_ID:
			{
				droneid = (droneid_track_info_packages_t *)targets;
				tracerGuideD(0, droneid);
				break;
			}
			case TRACK_INFO_FROM_USER:
			{
				user_target = (user_track_target_packages_t *)targets;
				g_nID = user_target->utt.objid;
				std::cout << "------------------2222 000000000000000000000000 c2 g_nID =" << g_nID << std::endl;
				StoppingTracking();
				return 2;
			}
			}
		}
	}

	// int BboxNum = output_stracks.size();
	// int ClassIndexs[10] = {-1, -1, -1,  -1, -1, -1,  -1, -1, -1,  -1};
	// int Boxes[40] = {0};
	// float Conf[10] = {0.4, 0.8, 0.9};

	for (int i = 0; i < output_stracks.size(); i++)
	{
		std::vector<float> tlwh = output_stracks[i].tlwh;

	// 	for (uint8_t i = 0; i < BboxNum; ++i)
	// 	{
	// 		Boxes[3 * i] = i * 10;
	// 		Boxes[3 * i + 1] = i * 10 + 1;
	// 		Boxes[3 * i + 2] = i * 10 + 2;
	// 		Boxes[3 * i + 3] = i * 10 + 3;
	// 	}

	// upload_target_info(BboxNum, ClassIndexs, Boxes, Conf, 0);

		if (tlwh[2] * tlwh[3] > 4)
		{
			// std::cout << " ID:  " << output_stracks[i].track_id << std::endl;
			// yolov8->draw_objects(img, objs, CLASS_NAMES, COLORS, output_stracks[i].track_id);
			float score = objs[i].prob;
			int label_id = objs[i].label;
			if (score >= 0.3 && label_id == 0)
			{
				std::cout << ("AI找到了无人机") << std::endl;
			}

			bool frameGuide = (img_count % 16 == 0) && (objs[i].label == 0);
			if (frameGuide)
			{
				StoppingTracking();

				double scale_w = 640.0 / img_w;
				double scale_h = 480.0 / img_h;
				int x1 = tlwh[0] * scale_w;
				int x2 = (tlwh[0] + tlwh[2]) * scale_w;
				int y1 = tlwh[1] * scale_h;
				int y2 = (tlwh[1] + tlwh[3]) * scale_h;

				PTZTracking(x1, x2, y1, y2);
			}
			if (isGuide == true && frameGuide == false)
			{
				StoppingTracking();

				double scale_w = 640.0 / img_w;
				double scale_h = 480.0 / img_h;
				int x1 = tlwh[0] * scale_w;
				int x2 = (tlwh[0] + tlwh[2]) * scale_w;
				int y1 = tlwh[1] * scale_h;
				int y2 = (tlwh[1] + tlwh[3]) * scale_h;

				PTZTracking(x1, x2, y1, y2);
				isGuide = false;
			}
		}
	}
	img_count++;

	// bool bShow=true;
	// if (bShow){
	// 	cv::resize(img, img, cv::Size(800, 640));
	// 	cv::imshow("frame", img);
	// }
	// int keyPressed = cv::waitKey(1);
	// if (keyPressed == 'q') // Exit loop if ESC key is pressed
	// continueCapturing = false;

	return 0;
}

int ai_alg_run2_id(cv::Mat &img, void *targets)
{
	radar_track_info_packages_t *radar_t = NULL;
	remoteid_track_info_packages_t *remoteid = NULL;
	droneid_track_info_packages_t *droneid = NULL;
	user_track_target_packages_t *user_target = NULL;
	track_info_packages_t *track_info = (track_info_packages_t *)targets; //(track_info_pac// tracerGuide(img_count, droneid);kages_t *)targets;
	int data_dev_type = 0;

	skyfend_log_info("%s:%d > g_bInit: %d", __FUNCTION__, __LINE__, g_bInit);

	if (!g_bInit)
	{
		yolov8->make_pipe(true);
		g_bInit = true;

		std::cout << "------------ 初次开始引导 --init---------------------------:  " << data_dev_type << std::endl;
		if (targets)
		{
			data_dev_type = track_info->track_pack_type;
			switch (data_dev_type)
			{
			case TRACK_INFO_RADAR:
			{
				radar_t = (radar_track_info_packages_t *)targets;
				radarGuide(0, radar_t, g_nID);
				break;
			}
			case TRACK_INFO_TRACER_P_REMOTE_ID:
			{
				remoteid = (remoteid_track_info_packages_t *)targets;
				tracerGuideR(0, remoteid);
				break;
			}
			case TRACK_INFO_TRACER_P_DRONE_ID:
			{
				droneid = (droneid_track_info_packages_t *)targets;
				tracerGuideD(0, droneid);
				break;
			}
			case TRACK_INFO_FROM_USER:
			{
				user_target = (user_track_target_packages_t *)targets;
				g_nID = user_target->utt.objid;
				std::cout << "------------------1111 000000000000000000000000 c2 g_nID =" << g_nID << std::endl;
				StoppingTracking();
				return 1;
			}
			}
		}
	}
	else
	{
		if (targets)
		{
			data_dev_type = track_info->track_pack_type;
			std::cout << "------------ else ----------------------------:  " << data_dev_type << std::endl;
			switch (data_dev_type)
			{
			case TRACK_INFO_FROM_USER:
			{
				user_target = (user_track_target_packages_t *)targets;
				g_nID = user_target->utt.objid;
				std::cout << "------------------1122 000000000000000000000000 c2 g_nID =" << g_nID << std::endl;
				StoppingTracking();
				break;
			}
			}
		}
	}

	int img_h = img.rows;
	int img_w = img.cols;

	cv::Size size = cv::Size{1280, 1280};
	yolov8->copy_from_Mat(img, size);
	yolov8->infer();
	std::vector<Object> objs;
	yolov8->postprocess(objs);
	std::vector<STrack> output_stracks = tracker->update(objs);

	if (0 == output_stracks.size())
		no_obj_count++;
	else
		no_obj_count = 0;

	std::cout << "11111111111111111111111111111111111111111111111111111111111111111 no_obj_count =" << no_obj_count << std::endl;
	skyfend_log_info("11111111111111111111111111111111111111111111111111111111111111111 no_obj_count = %d", no_obj_count);
	// if (no_obj_count > 16 || g_nID > -1)
	if (no_obj_count > 2)
	{
		std::cout << "++++++++++++++++++++++++ 控制过程中引导 ++++++++++++++++++++++++++++" << std::endl;
		no_obj_count = 0;
		isGuide = true;
		if (targets && isGuide)
		{
			data_dev_type = track_info->track_pack_type;
			std::cout << "333333333333333333333333333333333333333333333333333333333:   " << data_dev_type << std::endl;
			switch (data_dev_type)
			{
			case TRACK_INFO_RADAR:
			{
				radar_t = (radar_track_info_packages_t *)targets;
				radarGuide(0, radar_t, g_nID);
				break;
			}
			case TRACK_INFO_TRACER_P_REMOTE_ID:
			{
				remoteid = (remoteid_track_info_packages_t *)targets;
				tracerGuideR(0, remoteid);
				break;
			}
			case TRACK_INFO_TRACER_P_DRONE_ID:
			{
				droneid = (droneid_track_info_packages_t *)targets;
				tracerGuideD(0, droneid);
				break;
			}
			case TRACK_INFO_FROM_USER:
			{
				user_target = (user_track_target_packages_t *)targets;
				g_nID = user_target->utt.objid;
				std::cout << "------------------2222 000000000000000000000000 c2 g_nID =" << g_nID << std::endl;
				StoppingTracking();
				return 2;
			}
			}
		}
	}

	// int BboxNum = output_stracks.size();
	// int ClassIndexs[10] = {-1, -1, -1,  -1, -1, -1,  -1, -1, -1,  -1};
	// int Boxes[40] = {0};
	// float Conf[10] = {0.4, 0.8, 0.9};

	for (int i = 0; i < output_stracks.size(); i++)
	{
		std::vector<float> tlwh = output_stracks[i].tlwh;

	// 	for (uint8_t i = 0; i < BboxNum; ++i)
	// 	{
	// 		Boxes[3 * i] = i * 10;
	// 		Boxes[3 * i + 1] = i * 10 + 1;
	// 		Boxes[3 * i + 2] = i * 10 + 2;
	// 		Boxes[3 * i + 3] = i * 10 + 3;
	// 	}

	// upload_target_info(BboxNum, ClassIndexs, Boxes, Conf, 0);

		if (tlwh[2] * tlwh[3] > 4)
		{
			// std::cout << " ID:  " << output_stracks[i].track_id << std::endl;
			// yolov8->draw_objects(img, objs, CLASS_NAMES, COLORS, output_stracks[i].track_id);
			float score = objs[i].prob;
			int label_id = objs[i].label;
			if (score >= 0.3 && label_id == 0)
			{
				std::cout << ("AI找到了无人机") << std::endl;
			}

			bool frameGuide = (img_count % 16 == 0) && (objs[i].label == 0);
			if (frameGuide)
			{
				StoppingTracking();

				double scale_w = 640.0 / img_w;
				double scale_h = 480.0 / img_h;
				int x1 = tlwh[0] * scale_w;
				int x2 = (tlwh[0] + tlwh[2]) * scale_w;
				int y1 = tlwh[1] * scale_h;
				int y2 = (tlwh[1] + tlwh[3]) * scale_h;

				PTZTracking(x1, x2, y1, y2);
			}
			if (isGuide == true && frameGuide == false)
			{
				StoppingTracking();

				double scale_w = 640.0 / img_w;
				double scale_h = 480.0 / img_h;
				int x1 = tlwh[0] * scale_w;
				int x2 = (tlwh[0] + tlwh[2]) * scale_w;
				int y1 = tlwh[1] * scale_h;
				int y2 = (tlwh[1] + tlwh[3]) * scale_h;

				PTZTracking(x1, x2, y1, y2);
				isGuide = false;
			}
		}
	}
	img_count++;

	// bool bShow=true;
	// if (bShow){
	// 	cv::resize(img, img, cv::Size(800, 640));
	// 	cv::imshow("frame", img);
	// }
	// int keyPressed = cv::waitKey(1);
	// if (keyPressed == 'q') // Exit loop if ESC key is pressed
	// continueCapturing = false;

	return 0;
}


bool g_bFirst=true;
int ai_alg_run3(cv::Mat &img, void *targets)
{
	radar_track_info_packages_t *radar_t = NULL;
	remoteid_track_info_packages_t *remoteid = NULL;
	droneid_track_info_packages_t *droneid = NULL;
	user_track_target_packages_t *user_target = NULL;
	track_info_packages_t *track_info = (track_info_packages_t *)targets; //(track_info_pac// tracerGuide(img_count, droneid);kages_t *)targets;
	int data_dev_type = 0;

	skyfend_log_info("%s:%d > g_bInit: %d", __FUNCTION__, __LINE__, g_bInit);



	if (!g_bInit)
	{
		yolov8->make_pipe(true);
		g_bInit = true;
	}
	else
	{
		if (targets)
		{
			data_dev_type = track_info->track_pack_type;
			switch (data_dev_type)
			{
			case TRACK_INFO_FROM_USER:
			{
				user_target = (user_track_target_packages_t *)targets;
				g_nID = user_target->utt.objid;
				std::cout << "------------------1122 000000000000000000000000 c2 g_nID =" << g_nID << std::endl;
				// StoppingTracking();
			}
			}
		}
	}

	if (targets && g_bFirst)
	{
		g_bFirst=false;
		data_dev_type = track_info->track_pack_type;
		switch (data_dev_type)
		{
		case TRACK_INFO_RADAR:
		{
			radar_t = (radar_track_info_packages_t *)targets;
			radarGuide(0, radar_t, g_nID);
			break;
		}
		case TRACK_INFO_TRACER_P_REMOTE_ID:
		{
			remoteid = (remoteid_track_info_packages_t *)targets;
			tracerGuideR(0, remoteid);
			break;
		}
		case TRACK_INFO_TRACER_P_DRONE_ID:
		{
			droneid = (droneid_track_info_packages_t *)targets;
			tracerGuideD(0, droneid);
		}
		case TRACK_INFO_FROM_USER:
		{
			user_target = (user_track_target_packages_t *)targets;
			g_nID = user_target->utt.objid;
			std::cout << "------------------1111 000000000000000000000000 c2 g_nID =" << g_nID << std::endl;
			// StoppingTracking();
			return 1;
		}
		}
	}

	int img_h = img.rows;
	int img_w = img.cols;

	cv::Size size = cv::Size{1280, 1280};
	yolov8->copy_from_Mat(img, size);
	yolov8->infer();
	std::vector<Object> objs;
	yolov8->postprocess(objs);
	std::vector<STrack> output_stracks = tracker->update(objs);

	for (int i = 0; i < output_stracks.size(); i++)
	{
		img_count++;
		std::vector<float> tlwh = output_stracks[i].tlwh;

	// 	for (uint8_t i = 0; i < BboxNum; ++i)
	// 	{
	// 		Boxes[3 * i] = i * 10;
	// 		Boxes[3 * i + 1] = i * 10 + 1;
	// 		Boxes[3 * i + 2] = i * 10 + 2;
	// 		Boxes[3 * i + 3] = i * 10 + 3;
	// 	}

	// upload_target_info(BboxNum, ClassIndexs, Boxes, Conf, 0);

		if (tlwh[2] * tlwh[3] > 4)
		{
			// std::cout << " ID:  " << output_stracks[i].track_id << std::endl;
			// yolov8->draw_objects(img, objs, CLASS_NAMES, COLORS, output_stracks[i].track_id);
			float score = objs[i].prob;
			int label_id = objs[i].label;
			if (score >= 0.3 && label_id == 0)
			{
				std::cout << ("AI找到了无人机") << std::endl;
			}

			// bool frameGuide = (img_count % 16 == 0) && (objs[i].label == 0);
			bool frameGuide = objs[i].label == 0;
			if (img_count == 1)
			{
				// StoppingTracking();

				double scale_w = 640.0 / img_w;
				double scale_h = 480.0 / img_h;
				int x1 = tlwh[0] * scale_w;
				int x2 = (tlwh[0] + tlwh[2]) * scale_w;
				int y1 = tlwh[1] * scale_h;
				int y2 = (tlwh[1] + tlwh[3]) * scale_h;

				PTZTracking(x1, x2, y1, y2);
			}
		
		}
	}

	return 0;
}

// int g_nID = -1;
int g_count=0;
int ai_alg_run3_nj(cv::Mat &img, void *targets)
{
	radar_track_info_packages_t *radar_t = NULL;
	remoteid_track_info_packages_t *remoteid = NULL;
	droneid_track_info_packages_t *droneid = NULL;
	user_track_target_packages_t *user_target = NULL;
	track_info_packages_t *track_info = (track_info_packages_t *)targets; //(track_info_pac// tracerGuide(img_count, droneid);kages_t *)targets;
	int data_dev_type = 0;

	skyfend_log_info("%s:%d > g_bInit: %d", __FUNCTION__, __LINE__, g_bInit);

	if (0==g_count)
		StopSDT();

	// if (targets && (g_count < 50))
	if (targets )
	{
		StartSDT(100, 200, 0, 10);

		g_count++;
		data_dev_type = track_info->track_pack_type;
		std::cout << "--------------data_dev_type---g_count:" << data_dev_type << ","<<g_count << std::endl;
		switch (data_dev_type)
		{
		case TRACK_INFO_RADAR:
		{
			radar_t = (radar_track_info_packages_t *)targets;
			radarGuide(0, radar_t, g_nID);
			break;
		}
		case TRACK_INFO_TRACER_P_REMOTE_ID:
		{
			remoteid = (remoteid_track_info_packages_t *)targets;
			tracerGuideR(0, remoteid);
			break;
		}
		case TRACK_INFO_TRACER_P_DRONE_ID:
		{
			droneid = (droneid_track_info_packages_t *)targets;
			tracerGuideD_N(0, droneid);
			break;
		}
		case TRACK_INFO_FROM_USER:
		{
			user_target = (user_track_target_packages_t *)targets;
			g_nID = user_target->utt.objid;
			std::cout << "------------------1111 000000000000000000000000 c2 g_nID =" << g_nID << std::endl;
			StoppingTracking();
			return 1;
		}
		}
		sleep(1.5);
		
	}
	// else{
	// 	StartSDT(100, 200, 0, 10);
	// }

	// // StopSDT();
	static int flag = 0;
	// setPTZ0(p, t, obj_to_ptz.droneHeight, res[5]); 
	// StartSDT(10000, 15000, 0, 8000);
	// StartSDT(100, 300, 0, 80);
	// sleep(3);

	// StartSDT(100, 200, 0, 10);

	// if(flag == 0) {
	// 	StartSDT(10000, 15000, 0, 1000);
	// 	flag = 1;
	// }

	// if (1==flag){
	// 	StopSDT();
	// }

	return 0;
}

// by H. Chen, 2023/11/08
void SearchAndTracking()
{
	// strl_cmd = 5：水平搜索 ctrl_cmd = 6：俯仰搜索
	ptz_msg_track_t track = {0};
	track.ctrl_cmd = 1;
	track.reserver = 0;
	send_ptz_control_track(&track);
}

bool firstGuide = true;
int img_cnt = 0;
int prev_cnt = 0;
int track_cnt = 0;
int prev_mode = -1;
int ai_alg_run7(cv::Mat &img, void *targets)
{
	radar_track_info_packages_t *radar_t = NULL;
	remoteid_track_info_packages_t *remoteid = NULL;
	droneid_track_info_packages_t *droneid = NULL;
	user_track_target_packages_t *user_target = NULL;
	track_info_packages_t *track_info = (track_info_packages_t *)targets;

	++img_cnt;

	// 释放
	if (realtime_status.work_mode == 1)
	{
		StoppingTracking();
	}

	// 保存设备状态
	if (prev_mode = 2 && realtime_status.work_mode == 0)
	{
		firstGuide = true;
	}
	prev_mode = realtime_status.work_mode;

	// 连续跟踪次数
	if (realtime_status.work_mode == 2)
	{
		++track_cnt;
	}
	else
	{
		std::cout << "========================= 已连续跟踪" << track_cnt << "帧!" << std::endl;
		track_cnt = 0;
	}

	// 如果有tracer或者雷达目标，进行引导
	if (targets)
	{
		// 空闲状态引导，没有间隔
		if (realtime_status.work_mode == 0 && (firstGuide || (img_cnt - prev_cnt >= 6)))
		{
			int data_dev_type = track_info->track_pack_type;
			std::cout << "<<<<<============================= 初次引导" << std::endl;
			switch (data_dev_type)
			{
				case TRACK_INFO_RADAR:
				{
					radar_t = (radar_track_info_packages_t *)targets;
					radarGuide(0, radar_t);
					break;
				}
				case TRACK_INFO_TRACER_P_REMOTE_ID:
				{
					remoteid = (remoteid_track_info_packages_t *)targets;
					tracerGuideR(0, remoteid);
					break;
				}
				case TRACK_INFO_TRACER_P_DRONE_ID:
				{
					droneid = (droneid_track_info_packages_t *)targets;
					tracerGuideD(0, droneid);
					break;
				}
				case TRACK_INFO_FROM_USER:
				{
					user_target = (user_track_target_packages_t *)targets;
					break;
				}
			}

			prev_cnt = img_cnt;

			// 判断是否完成引导
			// (abs(realtime_pa_t.horizatal_angle - <) && (realtime_pa_t.pitching_angle - <))	

			// 光电上报信息中有目标，搜索并自动跟踪
			if (realtime_targets.target_num)
			{
				SearchAndTracking();
			}
		}
		// 跟踪状态引导
		if (realtime_status.work_mode == 2 && track_cnt && !(track_cnt % 30))
		{
			int data_dev_type = track_info->track_pack_type;
			std::cout << ">>>>>============================= 跟踪状态引导 " << std::endl;
			switch (data_dev_type)
			{
				case TRACK_INFO_RADAR:
				{
					radar_t = (radar_track_info_packages_t *)targets;
					radarGuide(0, radar_t);
					break;
				}
				case TRACK_INFO_TRACER_P_REMOTE_ID:
				{
					remoteid = (remoteid_track_info_packages_t *)targets;
					tracerGuideR(0, remoteid);
					break;
				}
				case TRACK_INFO_TRACER_P_DRONE_ID:
				{
					droneid = (droneid_track_info_packages_t *)targets;
					tracerGuideD(0, droneid);
				}
				case TRACK_INFO_FROM_USER:
				{
					user_target = (user_track_target_packages_t *)targets;
					break;
				}
			}
		}
	}

	return 0;
}