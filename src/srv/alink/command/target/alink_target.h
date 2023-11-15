
#ifndef __ALINK_TARGET_H__
#define __ALINK_TARGET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../alink.h"

#pragma pack(1)
typedef struct alink_targets_item
	{
		uint16_t  id;
		uint8_t  classification;
		uint16_t  rectX;
		uint16_t  rectY;
		uint16_t  rectW;
		uint16_t  rectH;
		uint16_t  rectXV;
		uint16_t  rectYV;
		int8_t  classfyProb;
		int16_t  reserve[4];
	}alink_targets_item_t;

	typedef struct alink_taget_info
	{
		uint32_t timestamp;
		uint8_t objNum;
		alink_targets_item_t targets[ALINK_TARGET_MAX];
	}alink_taget_info_t;
#pragma pack()

#pragma pack(1)
typedef struct user_track_target {
	uint8_t type;
	uint8_t objid;
	uint32_t x;
	uint32_t y;
	uint32_t z;
	uint8_t sn[25];
	uint32_t latitude;
	uint32_t longitde;
}user_track_target_t;
#pragma pack()

	typedef struct user_track_target_packages {
		int track_pack_type;
		user_track_target_t utt;
	}user_track_target_packages_t;

void alink_target_init( void );
int32_t alink_upload_target_info( void *psObjTargetInfo );

#ifdef __cplusplus
}
#endif

#endif /* ALINK_TARGET_H */
