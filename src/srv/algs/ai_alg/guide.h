// Test.cpp : Defines the entry point for the DLL application.
//
#include "config.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>
// #include <numpy/arrayobject.h>
#include <ctime>
// #include <json/json.h>
#include <thread>
#include "alink_track.h"

class RadarObj
{
public:
	RadarObj(int id = 0, int alive = 0, int stateType = 0, int threatLevel = 0);
	friend std::ostream &operator<<(std::ostream &os, const RadarObj &dt);
	int id;
	int alive;				// 生命周期
	int stateType;			// 0：暂态航迹  1：潜在的有用目标
	int threatLevel;		// 威胁等级 取值：1、2、3。1表示危险等级最高
	double azimuth = 0.0;	// 方位角
	double elevation = 0.0; // 俯仰角
	uint32_t range = 0;		//
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;
};
std::ostream &operator<<(std::ostream &os, const RadarObj &instance);
int radar_compare(RadarObj a, RadarObj b);
int radar_read(radar_track_info_packages_t *radar_targets, std::vector<RadarObj> &obj_list);
int radar_2ptz(std::vector<RadarObj> *obj_ptr, RadarObj &obj_to_ptz, int ptz_id);


class TracerObj
{
public:
	TracerObj(int uDroneNum = 0, int uDangerLevels = 0);
	friend std::ostream &operator<<(std::ostream &os, const TracerObj &dt);

	int uDroneNum=0;

	std::string droneName = "";
	double droneLongitude = 0.0;
	double droneLatitude = 0.0;
	double droneHeight = 0.0;
	double droneYawAngle = 0.0;
	double droneSpeed = 0.0;
    double droneVerticalSpeed = 0.0;

    double pilotLongitude = 0.0;
    double pilotLatitude = 0.0;

    int uFreq = 0;//无人机信号mHz
    double uDistance = 0.0;//无人机与tracer距离m
    int uDangerLevels=0;//危险等级
};

std::ostream &operator<<(std::ostream &os, const TracerObj &instance);

int tracer_compare(TracerObj a, TracerObj b);
int tracerR_read(remoteid_track_info_packages_t *tracer_targets ,std::vector<TracerObj> &obj_list);
int tracerD_read(droneid_track_info_packages_t *tracer_targets ,std::vector<TracerObj> &obj_list);

/*
返回值：
 - 0：没有目标
 - 1：有唯一目标
 - 2：有多个目标，给出威胁最大的目标
*/
int tracer_2ptz(std::vector<TracerObj> *obj_ptr, TracerObj &obj_to_ptz);


////////////////////////////////////////////////////////////////////////////////
int main_api(std::string& data_file, TracerObj& obj_to_ptz);
