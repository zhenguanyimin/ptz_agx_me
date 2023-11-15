
#include "alink_system.h"
#include "../../../../app/c2_network/c2_network.h"

static void system_set_status( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	alink_sys_set_status_t *psSetStatus;
	alink_sys_set_status_ack_t *psSetStatusAck;

	psSetStatus = (alink_sys_set_status_t*)(psPayload->pbyData);
	psSetStatusAck = (alink_sys_set_status_ack_t*)(psResp->pbyBuffer);
	switch( psSetStatus->type )
	{
		case 0:
			psSetStatusAck->status = 0;
			psResp->wCount = sizeof(alink_sys_set_status_ack_t);
			break;
		case 1:
			psSetStatusAck->status = 0;
			psResp->wCount = sizeof(alink_sys_set_status_ack_t);
			break;
		case 2:
			psSetStatusAck->status = 0;
			psResp->wCount = sizeof(alink_sys_set_status_ack_t);
			break;
		case 3:
			psSetStatusAck->status = 0;
			psResp->wCount = sizeof(alink_sys_set_status_ack_t);
			break;
		case 4:
			if( 0 == alink_set_upload_dev_id( psSetStatus->param1 ) )
				psSetStatusAck->status = 0;
			else
				psSetStatusAck->status = 1;
			psResp->wCount = sizeof(alink_sys_set_status_ack_t);
			break;
		default:
			break;
	}
}

static void system_get_info( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	alink_sys_get_info_t *psGetInfo;
	alink_sys_get_info_ack_t *psGetInfoAck;

	psGetInfo = (alink_sys_get_info_t*)(psPayload->pbyData);
	psGetInfoAck = (alink_sys_get_info_ack_t*)(psResp->pbyBuffer);
	switch( psGetInfo->type )
	{
		case 0:
			psGetInfoAck->type = 0;
			psGetInfoAck->value[0] = 0;
			psResp->wCount = sizeof(alink_sys_get_info_ack_t);
			break;
		case 1:
			psGetInfoAck->type = 1;
			psGetInfoAck->value[0] = 0;
			memset( &psGetInfoAck->value[1], 0, sizeof(psGetInfoAck->value) - 1 );
			strcpy( (char*)&psGetInfoAck->value[1], "RADAR_12345678" );
			psResp->wCount = sizeof(alink_sys_get_info_ack_t);
			break;
		case 2:
			psGetInfoAck->type = 2;
			*((uint32_t*)(psGetInfoAck->value)) = 3;
			psResp->wCount = sizeof(alink_sys_get_info_ack_t);
			break;
		default:
			break;
	}
}

static void system_heartbeat_ext( alink_payload_t *psPayload, alink_resp_t *psResp )
{
//	c2_network_heartbeat_ext();
}

static void system_set_sn( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	alink_sys_set_sn_t *psSetSn;
	alink_sys_set_sn_ack_t *psSetSnAck;

	psSetSn = (alink_sys_set_sn_t*)(psPayload->pbyData);
	psSetSnAck = (alink_sys_set_sn_ack_t*)(psResp->pbyBuffer);

	(void)psSetSn;
	(void)psSetSnAck;

	int eRet = -1;//FlashRaw_Set( "sn", psSetSn->sn, strlen(psSetSn->sn) );

	if( 0 > eRet )
	{
		psSetSnAck->status = 0;
	}
	else
	{
		psSetSnAck->status = 1;
	}
	psResp->wCount = sizeof(alink_sys_set_sn_ack_t);
}

// static void system_get_amp_code( alink_payload_t *psPayload, alink_resp_t *psResp )
// {
// 	uint16_t wLen = 48;

// 	get_amp_code_store( psResp->pbyBuffer, &wLen );
// 	printf( "[%s %d]\r\n", __func__, __LINE__ );
// 	for( int i = 0; wLen > i; ++i )
// 		printf( " %02X", psResp->pbyBuffer[i] );	
// 	printf( "\r\n" );
// 	psResp->wCount = wLen;
// }

// static void system_get_amp_data( alink_payload_t *psPayload, alink_resp_t *psResp )
// {
// 	uint16_t wLen = 768;

// 	get_amp_data_store( psResp->pbyBuffer, &wLen );
// 	printf( "[%s %d]\r\n", __func__, __LINE__ );
// 	for( int i = 0; wLen > i; ++i )
// 		printf( " %02X", psResp->pbyBuffer[i] );
// 	printf( "\r\n" );
// 	psResp->wCount = wLen;
// }

// static void system_get_phase_data( alink_payload_t *psPayload, alink_resp_t *psResp )
// {
// 	uint16_t wLen = 768;

// 	phase_data_store( psResp->pbyBuffer, &wLen );
// 	printf( "[%s %d]\r\n", __func__, __LINE__ );
// 	for( int i = 0; wLen > i; ++i )
// 		printf( " %02X", psResp->pbyBuffer[i] );
// 	printf( "\r\n" );
// 	psResp->wCount = wLen;
// }

extern uint8_t eSysStatus;
static uint16_t system_pkg_heartbeat( uint8_t *pbyBuffer, uint16_t wSize, void *psStatus )
{
	uint32_t i;
	alink_sys_heartbeat_t *psHeartBeat;

	psHeartBeat = (alink_sys_heartbeat_t*)pbyBuffer;

	psHeartBeat->electricity	= 100;
	psHeartBeat->status			= 0;//eSysStatus;	//need read
	psHeartBeat->faultnum		= 0;	//need read
	for( i = 0; psHeartBeat->faultnum > i; ++i )
	{
		psHeartBeat->faultCode[i] = 0;	//need read
	}

	return 3+i*4;
}

static alink_cmd_list_t sAlinkCmd_SystemSetStatus = { 0 };
static alink_cmd_list_t sAlinkCmd_SystemGetInfo = { 0 };
static alink_cmd_list_t sAlinkCmd_SystemHeartBeatExt = { 0 };
static alink_cmd_list_t sAlinkCmd_SystemSetSn = { 0 };

static alink_package_list_t sAlinkPackage_HeartBeat = { 0 };

void alink_system_init( void )
{
	sAlinkCmd_SystemSetStatus.cmd_type = ALINK_USER_TYPE_C2;
	sAlinkCmd_SystemGetInfo.cmd_type = ALINK_USER_TYPE_C2;
	sAlinkCmd_SystemHeartBeatExt.cmd_type = ALINK_USER_TYPE_C2;
	sAlinkCmd_SystemSetSn.cmd_type = ALINK_USER_TYPE_C2;
	alink_register_cmd( &sAlinkCmd_SystemSetStatus,		0xD1,	sizeof(alink_sys_set_status_t),		system_set_status );
	alink_register_cmd( &sAlinkCmd_SystemGetInfo,		0xD2,	sizeof(alink_sys_get_info_t),		system_get_info );
	alink_register_cmd( &sAlinkCmd_SystemHeartBeatExt,	0xD3,	sizeof(alink_sys_heartbeat_ext_t),	system_heartbeat_ext );
	alink_register_cmd( &sAlinkCmd_SystemSetSn,			0xD4,	sizeof(alink_sys_set_sn_t),			system_set_sn );

	alink_register_package( &sAlinkPackage_HeartBeat,	0xD0,	0, 									system_pkg_heartbeat );
}

int32_t alink_upload_heartbeat( void )
{
	alink_upload_package(&sAlinkPackage_HeartBeat, NULL);

	return 0;
}
