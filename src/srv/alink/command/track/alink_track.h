
#ifndef ALINK_TRACK_H
#define ALINK_TRACK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../alink.h"

#define ALINK_TARGET_PLUS_MAX	(128)

//#define DEBUG_TRACK_INFO// 
#define DEBUG_ALG_TRACK_INFO

typedef enum {
	TRACK_INFO_RADAR = 0,
	TRACK_INFO_TRACER_P_REMOTE_ID,
	TRACK_INFO_TRACER_P_DRONE_ID,
	TRACK_INFO_FROM_USER,
} TRACK_INFO_PACKAGE_TYPE;

#pragma pack(1)
	typedef struct alink_track_set_upload_mode
	{
		uint8_t  type;
		uint8_t  reserved;
		uint8_t  status;
		uint8_t  route;
		uint8_t  cycle;
	}alink_track_set_upload_mode_t;

	typedef struct alink_track_set_upload_mode_ack
	{
		uint8_t  status;
	}alink_track_set_upload_mode_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_track_get_config_info
	{
		uint8_t  reserved;
	}alink_track_get_config_info_t;

	typedef struct alink_track_get_config_info_ack
	{
		uint8_t  trSwitchCtrl;
		uint8_t  workMode;
		uint8_t  workWaveCode;
		uint8_t  workFreqCode;
		uint8_t  prfPeriod;
		uint8_t  accuNum;
		uint8_t  cohesionVelThre;
		uint8_t  cohesionRgnThre;
		uint8_t  clutterMapSwitch;
		uint8_t  clutterMapUpdateCoef;
		int8_t   aziCalcSlope;
		int8_t   aziCalcPhase;
		int8_t   eleCalcSlope;
		int8_t   eleCalcPhase;
		int8_t   aziScanCenter;
		uint8_t  aziScanScope;
		int8_t   eleScanCenter;
		uint8_t  eleScanScope;
		uint16_t coherentDetectSwitch;
		uint16_t noiseCoef;
		uint16_t clutterCoef;
		uint16_t cfarCoef;
		uint16_t focusRangeMin;
		uint16_t focusRangeMax;
		int16_t  clutterCurveNum;
		int16_t  lobeCompCoef;
	}alink_track_get_config_info_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_track_detected_item
	{
		uint8_t  id;
		uint8_t  reserved;
		uint8_t  ambiguous;
		uint8_t  classification;
		uint8_t  classfyProb;
		uint8_t  cohesionOkFlag;
		uint8_t  cohesionPntNum;
		uint8_t  cohesionBeamNum;
		int16_t  azimuth;
		int16_t  elevation;
		int16_t  velocity;
		int16_t  dopplerChn;
		uint16_t mag;
		uint16_t objConfidence;
		uint32_t range;
	}alink_track_detected_item_t;

	typedef struct alink_track_detected
	{
		uint8_t  detectObjNum;
		uint8_t  trackTwsTasFlag;
		uint8_t  detectObjByte;
		uint8_t  aziBeamID;
		uint8_t  eleBeamID;
		alink_track_detected_item_t detectPoint[ALINK_TARGET_MAX];
	}alink_track_detected_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_track_tracked_item
	{
		uint8_t  id;
		uint8_t  classification;
		uint8_t  classfyProb;
		uint8_t  existingProb;
		uint8_t  ambiguous;
		uint8_t  twsTasFlag;
		uint8_t  stateType;
		uint8_t  motionType;
		int16_t  azimuth;
		uint32_t range;
		int16_t  elevation;
		int16_t  velocity;
		int16_t  dopplerChn;
		uint16_t mag;
		int16_t  absVel;
		int16_t  orientationAngle;
		uint16_t alive;
		int32_t  x;
		int32_t  y;
		int32_t  z;
		int16_t  vx;
		int16_t  vy;
		int16_t  vz;
		int16_t  ax;
		int16_t  ay;
		int16_t  az;
		uint16_t x_variance;
		uint16_t y_variance;
		uint16_t z_variance;
		uint16_t vx_variance;
		uint16_t vy_variance;
		uint16_t vz_variance;
		uint16_t ax_variance;
		uint16_t ay_variance;
		uint16_t az_variance;
		uint16_t forcastFrameNum;
		uint16_t associationNum;
		uint32_t assocBit0;
		uint32_t assocBit1;
		uint16_t threatLevel;
		uint16_t reserve[3];		
	}alink_track_tracked_item_t;

	typedef struct alink_track_tracked
	{
		uint8_t  reserved;
		uint8_t  trackObjNum;
		uint8_t  trackTwsNum;
		uint8_t  trackTasNum;
		uint8_t  trackObjByte;
		alink_track_tracked_item_t trackPoint[ALINK_TARGET_MAX];
	}alink_track_tracked_t;

	typedef struct alink_track_tracked_plus
	{
		uint8_t  status;
		uint8_t  trackObjNum;
		uint8_t  trackTwsNum;
		uint8_t  trackTasNum;
		uint8_t  trackObjByte;
		uint64_t timestamp;
		alink_track_tracked_item_t trackPoint[ALINK_TARGET_PLUS_MAX];
	}alink_track_tracked_plus_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_track_beam_scheduling
	{
		uint8_t  reserved;
		uint8_t  aziBeamID;			//0-29
		uint8_t  eleBeamID;			//0-3
		uint8_t  trackTwsTasFlag;
		uint8_t  tasObjId;
		uint8_t  tasBeamTotal;
		uint8_t  tasBeamCntCur;
		int16_t  aziBeamSin;
		int16_t  eleBeamSin;
		uint16_t tasObjFilterNum;
		uint16_t tasObjRange;
		uint16_t samplePntStart;
		uint16_t samplePntDepth;
		uint16_t beamSwitchTime;
		uint32_t wholeSpaceScanCycleCnt;
	}alink_track_beam_scheduling_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_track_attitude
	{
		uint8_t	 reserved;
		uint32_t heading;
		uint32_t pitching;
		uint32_t rolling;
		uint32_t longitude;
		uint32_t latitude;
		uint32_t altitude;
		uint32_t velocityNavi;
		uint16_t targetTimeMark[6];
		uint32_t sigProcRelativeTime;
	}alink_track_attitude_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_drone_id_items {
		char productType;
		char droneName[25];
		char serialNum[32];
		int32_t droneLongitude;
		int32_t droneLatitude;
		int16_t droneHeight;
		int16_t droneDirection;
		int16_t droneSpeed;
		char Speedderection;
		int16_t droneVerticalSpeed;
		int32_t droneSailLongitude;
		int32_t droneSailLatitude;
		int32_t uFreq;
		int16_t uDistance;
		int16_t uDangerLevels;
	}alink_drone_id_items_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_drone_id_track {
		char sn_name[25];
		char uDroneNum;
		alink_drone_id_items_t items[ALINK_TARGET_MAX];
	} alink_drone_id_track_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_remote_id_items {
		char productType;
		char droneName[25];
		char serialNum[32];
		int32_t droneLongitude;
		int32_t droneLatitude;
		int16_t droneHeight;
		int16_t droneDirection;
		int16_t droneSpeed;
		char Speedderection;
		int16_t droneVerticalSpeed;
		int32_t droneSailLongitude;
		int32_t droneSailLatitude;
		int32_t uFreq;
		int16_t uDistance;
		int16_t uDangerLevels;
	}alink_remote_id_items_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_remote_id_track {
		char sn_name[25];
		char uDroneNum;
		alink_remote_id_items_t items[ALINK_TARGET_MAX];
	} alink_remote_id_track_t;
#pragma pack()

#pragma pack(1)
	typedef struct radar_track_info_packages {
		int track_pack_type;
		alink_track_tracked_t track_tracked;
		alink_track_attitude_t track_attitude;
	}radar_track_info_packages_t;
#pragma pack()

	typedef struct remoteid_track_info_packages {
		int track_pack_type;
		alink_remote_id_track_t remote_id;
	}remoteid_track_info_packages_t;

	typedef struct droneid_track_info_packages {
		int track_pack_type;
		alink_drone_id_track_t drone_id;
	}droneid_track_info_packages_t;

	typedef struct track_info_packages {
		int track_pack_type;
		char track_info[0];
		// radar_track_info_packages_t *radar_track_info;
		// tracerP_track_info_packages_t *tracerP_track_info;
	} track_info_packages_t;

void alink_track_init( void );
#ifdef __cplusplus
}
#endif

#endif /* ALINK_TRACK_H */
