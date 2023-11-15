#include <stdio.h>
#include <math.h>
#include <sys/time.h>

#include "alink_track.h"
#include "../../../algs/alg_interface.h"
#include "../../../eth_link/ptz_protocol/ptz_control.h"

// #define SUPPORT_RADAR_LEAD_PTZ_DIRECTLY
static radar_track_info_packages_t pack;

#ifdef SUPPORT_RADAR_LEAD_PTZ_DIRECTLY

#define EARTH_RADIUS 6371.0 // 地球半径，单位：千米
#define EARTH_PERIMETER 40075 //周长

double calculateDistance(int32_t x, int32_t y)
{
	double distance = sqrt(x * x + y * y);

    return distance;
}

static int cal_target_altitue_longitude(alink_track_attitude_t *track_attitude, int32_t x, int32_t y, double *latitude,\
										 double *longitude)
{
	double distance = 0;
	uint32_t radar_latitude = track_attitude->latitude;
	uint32_t radar_longitude = track_attitude->longitude;

	distance = calculateDistance(x, y);

	*latitude = radar_latitude + (distance/EARTH_PERIMETER) * 360;
	*longitude = radar_longitude + (distance/EARTH_PERIMETER) * 360 * cos(radar_latitude);

	return 0;
}

static int lead_ptz_by_target_infos(radar_track_info_packages_t *target_info)
{
	int loop = 0;
	double latitude = 0;
	double longitude = 0;
	double altitude = target_info->track_attitude.altitude;
	ptz_msgid_0x03_addr_t addr_t = {0};
	struct timeval tv;

	for(loop = 0; loop < target_info->track_tracked.trackObjNum; loop++) {
		cal_target_altitue_longitude(&(target_info->track_attitude), target_info->track_tracked.trackPoint[loop].x,\
								 	target_info->track_tracked.trackPoint[loop].y, &latitude, &longitude);
		gettimeofday(&tv, NULL);
		addr_t.photoelectricity_no = PTZ_PHOTOELECTRICITY_NO;
		addr_t.sys_no = PTZ_SYS_NO;
		addr_t.sys_timesmap = tv.tv_sec * 1000 + tv.tv_usec / 1000;
		addr_t.target_longitude = longitude;
		addr_t.target_latitude = latitude;
		addr_t.target_altitude = target_info->track_tracked.trackPoint[loop].z + altitude;
		addr_t.user_id = PTZ_USER_ID;
		addr_t.guidance_mode = PTZ_ADDR_GUIDANCE_MODE_GEOGRAPHY;
		addr_t.target_motion_direction = target_info->track_tracked.trackPoint[loop].motionType;

		send_ptz_control_addr(&addr_t);
	}

	return 0;
}
#endif

static void track_pkg_tracked( alink_payload_t *psPayload, alink_resp_t *psResp )
{

#ifdef DEBUG_TRACK_INFO
	char *ptmp = (char *)psPayload->pbyData;
	printf("%s:%d>portid =%d, payload len = %d\n", __FUNCTION__, __LINE__, psPayload->ePortId, psPayload->wLength);
	for(int i=0; i<psPayload->wLength; i++) {
		printf("%02x ", ptmp[i]);
	}
	printf("\n");
#endif

	memcpy(&(pack.track_tracked), psPayload->pbyData, psPayload->wLength);
	pack.track_pack_type = TRACK_INFO_RADAR;
	msg_send_to_alg(&pack, sizeof(radar_track_info_packages_t));

#ifdef SUPPORT_RADAR_LEAD_PTZ_DIRECTLY
	lead_ptz_by_target_infos(&pack);
#endif

}

static void track_pkg_attitude( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	
#ifdef DEBUG_TRACK_INFO
	char *ptmp = (char *)psPayload->pbyData;
	skyfend_log_debug("%s:%d> payload len = %d", __FUNCTION__, __LINE__, psPayload->wLength);
	for(int i=0; i<psPayload->wLength; i++) {
		printf("%02x ", ptmp[i]);
	}
	printf("\n");
#endif

	memcpy(&(pack.track_attitude), psPayload->pbyData, psPayload->wLength);
}

static void track_pkg_drone_id( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	
#ifdef DEBUG_TRACK_INFO
	char *ptmp = (char *)psPayload->pbyData;
	skyfend_log_debug("%s:%d> payload len = %d", __FUNCTION__, __LINE__, psPayload->wLength);
	for(int i=0; i<psPayload->wLength; i++) {
		printf("%02x ", ptmp[i]);
	}
	printf("\n");
#endif
	droneid_track_info_packages_t droneid_pack = {0};
	alink_drone_id_track_t *droneId= (alink_drone_id_track_t *)psPayload->pbyData;

	droneid_pack.track_pack_type = TRACK_INFO_TRACER_P_DRONE_ID;
	memcpy(&droneid_pack.drone_id, droneId, psPayload->wLength);
	msg_send_to_alg(&droneid_pack, sizeof(droneid_track_info_packages_t));
}

static void track_pkg_remote_id( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	
#ifdef DEBUG_TRACK_INFO
	char *ptmp = (char *)psPayload->pbyData;
	skyfend_log_debug("%s:%d> payload len = %d", __FUNCTION__, __LINE__, psPayload->wLength);
	for(int i=0; i<psPayload->wLength; i++) {
		printf("%02x ", ptmp[i]);
	}
	printf("\n");
#endif
	remoteid_track_info_packages_t remoteid_pack = {0};
	alink_remote_id_track_t *remoteid= (alink_remote_id_track_t *)psPayload->pbyData;

	remoteid_pack.track_pack_type = TRACK_INFO_TRACER_P_REMOTE_ID;
	memcpy(&remoteid_pack.remote_id, remoteid, psPayload->wLength);
	msg_send_to_alg(&remoteid_pack, sizeof(int)+psPayload->wLength);
}

static alink_cmd_list_t sAlinkPackage_Tracked = { 0 };
static alink_cmd_list_t sAlinkPackage_Attitude = { 0 };
static alink_cmd_list_t sAlinkPackage_remoteid = { 0 };
static alink_cmd_list_t sAlinkPackage_droneid = { 0 };

void alink_track_init(void)
{
	sAlinkPackage_Tracked.cmd_type = ALINK_USER_TYPE_DEVICE;
	sAlinkPackage_Attitude.cmd_type = ALINK_USER_TYPE_DEVICE;
	sAlinkPackage_remoteid.cmd_type = ALINK_USER_TYPE_DEVICE;
	sAlinkPackage_droneid.cmd_type = ALINK_USER_TYPE_DEVICE;
	alink_register_cmd( &sAlinkPackage_Tracked, 		0x12,	sizeof(alink_track_get_config_info_t),	track_pkg_tracked );
	alink_register_cmd( &sAlinkPackage_Attitude, 		0x13,	sizeof(alink_track_get_config_info_t),	track_pkg_attitude );
	alink_register_cmd( &sAlinkPackage_droneid, 		0xE0,	sizeof(alink_track_get_config_info_t),	track_pkg_drone_id );
	alink_register_cmd( &sAlinkPackage_remoteid, 		0xE1,	sizeof(alink_track_get_config_info_t),	track_pkg_remote_id );
}
