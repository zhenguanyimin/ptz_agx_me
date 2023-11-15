
#include "alink.h"
#include "analysis/alink_order.h"
#include "analysis/alink_recv.h"
#include "analysis/alink_socket.h"
#include "upload/alink_package.h"
#include "upload/alink_upload.h"
#include "command/track/alink_track.h"
#include "command/target/alink_target.h"
#include "../../inc/skyfend_error.h"
#include "../../srv/log/skyfend_log.h"
#include "command/system/alink_system.h"

//radar -> agx
static uint8_t abySendBuffer_server[ALINK_SEND_BUFFER_SIZE] = { 0 };
static alink_order_t sOrder_server	= { 0 };

//agx -> c2
static uint8_t abySendBuffer[ALINK_SEND_BUFFER_SIZE] = { 0 };
static alink_order_t sOrder	= { 0 };
static uint8_t abyUploadBuffer[ALINK_UPLOAD_BUFFER_SIZE] = { 0 };
static alink_upload_t sUpload = { 0 };

int32_t alink_init( void )
{
	int32_t eRet = 0;

	alink_order_init( &sOrder, abySendBuffer, sizeof(abySendBuffer) );
	alink_order_init( &sOrder_server, abySendBuffer_server, sizeof(abySendBuffer_server) );
	alink_upload_init( &sUpload, ALINK_DEV_ID_AGX, abyUploadBuffer, sizeof(abyUploadBuffer) );
	alink_track_init();
	alink_target_init();
	alink_system_init();

	return eRet;
}

int32_t alink_connect_port(alink_socket_t *psSocket, alink_port_t *psPort, uint8_t type)
{
	int32_t eRet = 0;

	if(ALINK_USER_TYPE_C2 == type) {
		eRet = alink_socket_init( psSocket, &sOrder );
	} else if(ALINK_USER_TYPE_DEVICE == type) {
		eRet = alink_socket_init( psSocket, &sOrder_server );
	} else {
		skyfend_log_error("%s:%d> cmd type invalid %d\n", __FUNCTION__, __LINE__, type);
		eRet = RET_VALUE_INVALID_PARM;
	}

	return eRet;
}

int32_t alink_register_cmd( alink_cmd_list_t *psCmdList, uint8_t byOrder, uint16_t wPayloadLen,
								void (*pv_cmd_task)(alink_payload_t *psPayload, alink_resp_t *psResp) )
{
	int32_t eRet = 0;
	alink_order_t *cur_sorder = NULL;

	psCmdList->byCmd		= byOrder;
	psCmdList->wPayloadLen	= wPayloadLen;
	psCmdList->pv_cmd_task	= pv_cmd_task;
	psCmdList->psNext		= NULL;
	if(ALINK_USER_TYPE_C2 == psCmdList->cmd_type) {
		cur_sorder = &sOrder;
	} else if(ALINK_USER_TYPE_DEVICE == psCmdList->cmd_type) {
		cur_sorder = &sOrder_server;
	} else {
		skyfend_log_error("%s:%d> cmd type invalid %d\n", __FUNCTION__, __LINE__, psCmdList->cmd_type);
		return RET_VALUE_INVALID_PARM;
	}

	eRet = alink_order_register_cmd(cur_sorder, psCmdList);

	return eRet;
}

int32_t alink_upload_package( alink_package_list_t *psPackage, void *psImport )
{
	return alink_upload_send_package( &sUpload, psPackage, psImport );
}

int32_t alink_register_package( alink_package_list_t *psPackageList, uint8_t byMsgId, uint8_t byAns, 
									uint16_t (*pw_package_task)(uint8_t* pbyBuffer, uint16_t wSize, void *psImport) )
{
	int32_t eRet = 0;

	eRet = alink_package_init( psPackageList, byMsgId, byAns, pw_package_task );

	eRet = alink_upload_register_package( &sUpload, psPackageList );

	return eRet;
}

int32_t alink_connect_send( alink_send_list_t *psSendList, uint8_t byMsgId, uint8_t byRemoteId, void *handle,
								int32_t (*pe_send)(void* handle, uint8_t *pbyData, uint32_t uLen) )
{
	psSendList->byRemoteId	= byRemoteId;
	psSendList->handle		= handle;
	psSendList->pe_send		= pe_send;

	return alink_upload_connect_send( &sUpload, byMsgId, psSendList );
}

int32_t alink_disconnect_send( alink_send_list_t *psSendList, uint8_t byMsgId )
{
	return alink_upload_disconnect_send( &sUpload, byMsgId, psSendList );
}

int32_t alink_set_upload_dev_id( uint8_t byDevId )
{
	if( ALINK_DEV_ID_RADAR == (0x0F & byDevId) )
	{
		sUpload.byDevId = byDevId;
		return 0;
	}
	else
	{
		return -1;
	}
}
