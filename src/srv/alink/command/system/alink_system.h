
#ifndef ALINK_SYSTEM_H
#define ALINK_SYSTEM_H

#include "../../alink.h"

#pragma pack(1)
	typedef struct alink_sys_set_status
	{
		uint8_t	 type;
		uint8_t  param1;
		uint8_t  param2;
	}alink_sys_set_status_t;

	typedef struct alink_sys_set_status_ack
	{
		uint8_t  status;
	}alink_sys_set_status_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_sys_get_info
	{
		uint8_t  type;
	}alink_sys_get_info_t;

	typedef struct alink_sys_get_info_ack
	{
		uint8_t	 type;
		uint8_t  value[16];
	}alink_sys_get_info_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_sys_heartbeat_ext
	{
		uint8_t sum;
	}alink_sys_heartbeat_ext_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_sys_set_sn
	{
		uint8_t sn[15];
	}alink_sys_set_sn_t;

	typedef struct alink_sys_set_sn_ack
	{
		uint8_t status;
	}alink_sys_set_sn_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_sys_heartbeat
	{
		uint8_t  electricity;
		uint8_t  status;
		uint8_t  faultnum;
		uint32_t faultCode[64];
	}alink_sys_heartbeat_t;
#pragma pack()

void alink_system_init( void );

#endif /* ALINK_SYS_H */
