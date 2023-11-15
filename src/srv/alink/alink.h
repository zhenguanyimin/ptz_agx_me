
#ifndef _ALINK_H_
#define _ALINK_H_

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "alink_defines.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define ALINK_TARGET_MAX		(128)

typedef enum alink_dev_id
{
	ALINK_DEV_ID_RADAR		= (3),
	ALINK_DEV_ID_PC			= (4),
	ALINK_DEV_ID_C2_BLE		= (5),
	ALINK_DEV_ID_C2			= (6),
	ALINK_DEV_ID_DRONE_ID	= (7),
	ALINK_DEV_ID_AGX		= (8),
}alink_dev_id_t;

typedef enum alink_port_id
{
	ALINK_PORT_ID_TCP_SRC	= (1),
	ALINK_PORT_ID_WIFI		= (2),
	ALINK_PORT_ID_TCP_C2	= (3),
}alink_port_id_t;

int32_t alink_init( void );

void alink_socket_task( alink_socket_t *psSocket, alink_recv_import_t *psImport );

int32_t alink_connect_port( alink_socket_t *psSocket, alink_port_t *psPort, uint8_t type);

int32_t alink_register_cmd( alink_cmd_list_t *psCmdList, uint8_t byOrder, uint16_t wPayloadLen,
								void (*pv_cmd_task)(alink_payload_t *psPayload, alink_resp_t *psResp) );

int32_t alink_upload_package( alink_package_list_t *psPackage, void *psImport );

int32_t alink_register_package( alink_package_list_t *psPackageList, uint8_t byMsgId, uint8_t byAns, 
									uint16_t (*pw_package_task)(uint8_t* pbyBuffer, uint16_t wSize, void *psImport) );

int32_t alink_connect_send( alink_send_list_t *psSendList, uint8_t byMsgId, uint8_t byRemoteId, void *handle,
								int32_t (*pe_send)(void* handle, uint8_t *pbyData, uint32_t uLen) );

int32_t alink_disconnect_send( alink_send_list_t *psSendList, uint8_t byMsgId );

int32_t alink_set_upload_dev_id( uint8_t byDevId );

/* upload function */
int32_t alink_upload_calibrite_data( void *psCaliInfo );

int32_t alink_upload_heartbeat( void );

int32_t alink_upload_detected( void *psObjDetect );
	
int32_t alink_upload_tracked( void *psObjTrack );
	
int32_t alink_upload_beam_scheduling( void *psObjBeamScheduling );
	
int32_t alink_upload_attitude( void *psObjPlatfrom );

int32_t alink_upload_req_channel( void );
	
int32_t alink_upload_connect_status( uint8_t byState );

int32_t alink_get_header_index(void *adat, uint32_t datlen);

int32_t alink_package_integrity_check(void *adat, uint32_t datlen);

#ifdef __cplusplus
}
#endif

#endif
