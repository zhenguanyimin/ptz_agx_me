// Test.cpp : Defines the entry point for the DLL application.
//
#include <math.h>
#include "guide.h"

RadarObj::RadarObj(int id, int alive, int stateType, int threatLevel) : id(id), alive(alive), stateType(stateType), threatLevel(threatLevel) {}

std::ostream &operator<<(std::ostream &os, const RadarObj &instance)
{
	os << "id:" << instance.id << "  "
	   << "alive:" << instance.alive << "  "
	   << "stateType: " << instance.stateType << "  "
	   << "threatLevel: " << instance.threatLevel << "  "
	   << "azimuth: " << instance.azimuth << "  "
	   << "elevation: " << instance.elevation << "  "
	   << "range: " << instance.range << "  "
	   << std::endl;
	return os;
}

int radar_compare(RadarObj a, RadarObj b)
{
	return a.threatLevel < b.threatLevel;
}

double g_dScale = 1.0/64;//2^6  by cdx 20231106
int radar_read(radar_track_info_packages_t *radar_targets,std::vector<RadarObj> &obj_list){

	for (int i = 0; i<radar_targets->track_tracked.trackObjNum; i++)
	{
		RadarObj tmp_obj(radar_targets->track_tracked.trackPoint[i].id, radar_targets->track_tracked.trackPoint[i].alive, \
						radar_targets->track_tracked.trackPoint[i].stateType, radar_targets->track_tracked.trackPoint[i].threatLevel);

		tmp_obj.azimuth = radar_targets->track_tracked.trackPoint[i].azimuth * g_dScale;
		tmp_obj.elevation = radar_targets->track_tracked.trackPoint[i].elevation * g_dScale;
		tmp_obj.x = radar_targets->track_tracked.trackPoint[i].x * g_dScale;
		tmp_obj.y = radar_targets->track_tracked.trackPoint[i].y * g_dScale;
		tmp_obj.z = radar_targets->track_tracked.trackPoint[i].y * g_dScale;
		tmp_obj.range = radar_targets->track_tracked.trackPoint[i].range * g_dScale;
		obj_list.push_back(tmp_obj);
	}

	return 0;
}

// int radar_read(radar_track_info_packages_t *radar_targets,std::vector<RadarObj> &obj_list){

// 	for (int i = 0; i<radar_targets->track_tracked.trackObjNum; i++)
// 	{
// 		RadarObj tmp_obj(radar_targets->track_tracked.trackPoint[i].id, radar_targets->track_tracked.trackPoint[i].alive, \
// 						radar_targets->track_tracked.trackPoint[i].stateType, radar_targets->track_tracked.trackPoint[i].threatLevel);

// 		tmp_obj.azimuth = radar_targets->track_tracked.trackPoint[i].azimuth;
// 		tmp_obj.elevation = radar_targets->track_tracked.trackPoint[i].elevation;
// 		tmp_obj.x = radar_targets->track_tracked.trackPoint[i].x;
// 		tmp_obj.y = radar_targets->track_tracked.trackPoint[i].y;
// 		tmp_obj.z = radar_targets->track_tracked.trackPoint[i].y;
// 		tmp_obj.range = radar_targets->track_tracked.trackPoint[i].range;
// 		obj_list.push_back(tmp_obj);
// 	}

// 	return 0;
// }

/*
返回值：
 - 0：没有目标
 - 1：有唯一目标
 - 2：有多个目标，给出威胁最大的目标
*/
extern int g_nID;
int radar_2ptz(std::vector<RadarObj> *obj_ptr, RadarObj &obj_to_ptz, int ptz_id=-1)
{
	std::vector<RadarObj> useful_obj;

	// 1.遍历所有目标，选取符合要求的目标，存入useful_obj中
	for (auto it = obj_ptr->begin(); it != obj_ptr->end(); ++it)
	{
		if ((it->stateType == 1) && (it->alive >= 10))
		{
			useful_obj.push_back(*it);
			// std::cout << it->alive << " " << it->stateType << " " << it->threatLevel << std::endl;
		}
	}

	// 2.判断是否有合适的目标
	// 2.1 未选取有效的引导目标
	if (useful_obj.size() == 0)
	{
		g_nID=-1;
		std::cout << "radar_2ptz 000 000000000000000000000000 c2 g_nID:" << g_nID << ",ptz_id:" << ptz_id << std::endl;

		return 0;
	}
	// 2.2 选定了有效的单个引导目标
	else if (useful_obj.size() == 1)
	{
		obj_to_ptz = useful_obj[0];
		if (obj_to_ptz.id != ptz_id)
			g_nID=-1;
		std::cout << "radar_2ptz 111 000000000000000000000000 c2 g_nID:" << g_nID << ",ptz_id:" << ptz_id << std::endl;

		return 1;
	}
	// 2.3 对目标的威胁等级进行排序
	else
	{
		// std::cout << "before sort:" << std::endl;
		// for(auto it =useful_obj.begin(); it != useful_obj.end(); ++it){
		//    std::cout << *it << std::endl;
		// }
		sort(useful_obj.begin(), useful_obj.end(), radar_compare);
		std::cout << "---------------------------------------------------------- after sort:" << useful_obj.size() << std::endl;		
		
		if (ptz_id > -1){		
			for(int i=0;i<useful_obj.size();i++){
				std::cout << useful_obj[i].id << std::endl;
				if ( ptz_id == useful_obj[i].id)
					obj_to_ptz = useful_obj[i];
			}
		}		
		else{
			obj_to_ptz = useful_obj[0];
		}

		if (obj_to_ptz.id != ptz_id)
			g_nID=-1;

		std::cout << "radar_2ptz 222 000000000000000000000000 c2 g_nID:" << g_nID << ",ptz_id:" << ptz_id << std::endl;
		return 2;
	}
}


////////////////////////////////////////////////////////////////////////
TracerObj::TracerObj(int uDroneNum, int uDangerLevels) : uDroneNum(uDroneNum), uDangerLevels(uDangerLevels) {}

std::ostream &operator<<(std::ostream &os, const TracerObj &instance)
{
	os << "uDroneNum:" << instance.uDroneNum << "  "
	   << "droneLongitude:" << instance.droneLongitude << "  "
	   << "droneLatitude: " << instance.droneLatitude << "  "
	   << "droneHeight: " << instance.droneHeight << "  "
	   << "droneYawAngle: " << instance.droneYawAngle << "  "
	   << "droneSpeed: " << instance.droneSpeed << "  "
	   << "pilotLongitude: " << instance.pilotLongitude << "  "
	   << "pilotLatitude: " << instance.pilotLatitude << "  "
	   << "uFreq: " << instance.uFreq << "  "
	   << "uDistance: " << instance.uDistance << "  "
	   << "uDangerLevels: " << instance.uDangerLevels << "  "
	   << std::endl;
	return os;
}

int tracer_compare(TracerObj a, TracerObj b)
{
	return a.uDangerLevels < b.uDangerLevels;
}

int tracerR_read(remoteid_track_info_packages_t *tracer_targets,std::vector<TracerObj> &obj_list)
{
	char serialNum[100] = "";//cdx
	// if(tracer_targets->remote_id.items->serialNum == serialNum){
		for(int i = 0; i < tracer_targets->remote_id.uDroneNum; i++)
		{
			TracerObj tmp_obj(tracer_targets->remote_id.uDroneNum, tracer_targets->remote_id.items[i].uDangerLevels);
			tmp_obj.droneName = tracer_targets->remote_id.items[i].droneName;
			tmp_obj.uDistance = tracer_targets->remote_id.items[i].uDistance;

			tmp_obj.droneLongitude = tracer_targets->remote_id.items[i].droneLongitude /1e7;
			tmp_obj.droneLatitude = tracer_targets->remote_id.items[i].droneLatitude /1e7;
			tmp_obj.droneHeight = tracer_targets->remote_id.items[i].droneHeight * 0.1;
			tmp_obj.droneSpeed = tracer_targets->remote_id.items[i].droneSpeed * 0.01;
			tmp_obj.droneVerticalSpeed = tracer_targets->remote_id.items[i].droneVerticalSpeed* 0.01;
			obj_list.push_back(tmp_obj);
		}
	// }

	return 0;
}

int tracerD_read(droneid_track_info_packages_t *tracer_targets,std::vector<TracerObj> &obj_list)
{
	char serialNum[100] = "F67PB237C003010K";//cdx
	// if(tracer_targets->drone_id.items->serialNum == serialNum){
		for(int i = 0; i < tracer_targets->drone_id.uDroneNum; i++)
		{
			TracerObj tmp_obj(tracer_targets->drone_id.uDroneNum, tracer_targets->drone_id.items[i].uDangerLevels);
			tmp_obj.droneName = tracer_targets->drone_id.items[i].droneName;
			tmp_obj.uDistance = tracer_targets->drone_id.items[i].uDistance;

			tmp_obj.droneLongitude = tracer_targets->drone_id.items[i].droneLongitude /(1e7*M_PI)*180;
			tmp_obj.droneLatitude = tracer_targets->drone_id.items[i].droneLatitude  /(1e7*M_PI)*180 ;///(1e7*180)*M_PI;///1e7/pi*180;
			tmp_obj.droneHeight = tracer_targets->drone_id.items[i].droneHeight*0.1;
			tmp_obj.droneSpeed = tracer_targets->drone_id.items[i].droneSpeed *0.01;
			tmp_obj.droneVerticalSpeed = tracer_targets->drone_id.items[i].droneVerticalSpeed *0.01;
			obj_list.push_back(tmp_obj);
		}
	// }

	return 0;
}

/*
返回值：
 - 0：没有目标
 - 1：有唯一目标
 - 2：有多个目标，给出威胁最大的目标
*/
int tracer_2ptz(std::vector<TracerObj> *obj_ptr, TracerObj &obj_to_ptz)
{
	std::vector<TracerObj> useful_obj;

	// 1.遍历所有目标，选取符合要求的目标，存入useful_obj中
	for (auto it = obj_ptr->begin(); it != obj_ptr->end(); ++it)
	{
			useful_obj.push_back(*it);
			// std::cout << it->alive << " " << it->stateType << " " << it->threatLevel << std::endl;
	}

	// 2.判断是否有合适的目标
	// 2.1 未选取有效的引导目标
	if (useful_obj.size() == 0)
	{
		return 0;
	}
	// 2.2 选定了有效的单个引导目标
	else if (useful_obj.size() == 1)
	{
		obj_to_ptz = useful_obj[0];
		// std::cout << "idididddddddddddddddddddddddddddddddddddddd: " << obj_to_ptz.id << std::endl;
		// std::cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx: " << obj_to_ptz.x << std::endl;
		// std::cout << "yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy: " << obj_to_ptz.y << std::endl;
		// std::cout << "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz: " << obj_to_ptz.z << std::endl;
		// std::cout << "++++++++++++++++++++++++++++++++++++++++++++range: " << obj_to_ptz.range << std::endl;
		// std::cout << "--------------------------------------------azimuth: " << obj_to_ptz.azimuth << std::endl;
		// std::cout << "********************************************elevation: " << obj_to_ptz.elevation << std::endl;
		return 1;
	}
	// 2.3 对目标的威胁等级进行排序
	else
	{
		std::cout << "before sort:" << std::endl;
		for(auto it =useful_obj.begin(); it != useful_obj.end(); ++it){
		   std::cout << *it << std::endl;
		}
		//sort(useful_obj.begin(), useful_obj.end(), tracer_compare);//cdx
		std::cout << "---------------------------------------------------------- after sort:" << useful_obj.size() << std::endl;
		// for(int i=0;i<useful_obj.size();i++){
		//	std::cout << "idididddddddddddddddddddddddddddddddddddddd: " << useful_obj[i].id << std::endl;
		//	std::cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx: " << useful_obj[i].x << std::endl;
		//	std::cout << "yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy: " << useful_obj[i].y << std::endl;
		//	std::cout << "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz: " << useful_obj[i].z << std::endl;
		//	std::cout << "++++++++++++++++++++++++++++++++++++++++++++range: " << useful_obj[i].range << std::endl;
		//	std::cout << "--------------------------------------------azimuth: " << useful_obj[i].azimuth << std::endl;
		//	std::cout << "********************************************elevation: " << useful_obj[i].elevation << std::endl;
		// }

		// for(auto it =useful_obj.begin(); it != useful_obj.end(); ++it){
		//     std::cout << *it << std::endl;
		// }
		obj_to_ptz = useful_obj[0];
		return 2;
	}
}

// int main_api(std::string& data_file, TracerObj& obj_to_ptz)
// {
// 	//std::string data_file="../radar.txt";
// 	std::vector<TracerObj> obj_list;
// 	tracer_read(data_file,obj_list);

// 	//TracerObj obj_to_ptz;
// 	int ret = tracer_2ptz(&obj_list, obj_to_ptz);

// 	return 0;
// }

// int main0(int argc, char **argv)
// {
// 	std::string data_file="../guide.txt";
// 	std::vector<TracerObj> obj_list;
// 	tracer_read(data_file,obj_list);

// 	TracerObj obj_to_ptz;
// 	int ret = tracer_2ptz(&obj_list, obj_to_ptz);

// 	return 0;
// }
