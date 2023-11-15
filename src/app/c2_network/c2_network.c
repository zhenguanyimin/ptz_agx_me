
#include <mqueue.h>
#include <sys/time.h>

#include "c2_network.h"
#include "../../srv/eth_link/eth_link.h"
#include "../../srv/alink/alink.h"
#include "../../srv/log/skyfend_log.h"
#include "../../common/tools/skyfend_tools.h"

typedef struct c2_network
{
	alink_send_list_t sSendAlinkHeartBeat;
	alink_send_list_t sSendAlinkTargetInfo;
	alink_socket_t sAlinkSocket;

	eth_link_cbk_t	sCbk;
	eth_link_user_t *psUser;
}c2_network_t;

int32_t c2_network_send( void* handle, uint8_t *pbyData, uint32_t uLen )
{
	eth_link_user_t *psUser = (eth_link_user_t*)handle;
	return	psUser->pi_send( psUser->psSocket, pbyData, uLen );
}

static void c2_network_cbk_connect(void *psHandle)
{
	skyfend_log_info( "[%s %d]\r\n", __func__, __LINE__ );
}
static void c2_network_cbk_data(void *psHandle,void *psClient,int32_t (*pi_recv)(void *psClient,void *pvBuffer,uint32_t uSize))
{
	c2_network_t *psNetwork = (c2_network_t*)psHandle;

	uint8_t abyBuffer[1024*1024*1];
	int32_t iLength = pi_recv( psClient, abyBuffer, sizeof(abyBuffer) );
	skyfend_log_debug( "[%s %d] %d\r\n", __func__, __LINE__, iLength );
	alink_recv_import_t sImport;
	sImport.ePortId	= 1;
	sImport.pbyData	= abyBuffer;
	sImport.uLength	= iLength;
	sImport.handle	= psNetwork->psUser->psSocket;
	sImport.pe_send	= psNetwork->psUser->pi_send;
	alink_socket_task( &psNetwork->sAlinkSocket, &sImport );
}
static void c2_network_cbk_disconnect(void *psHandle)
{
	skyfend_log_info( "[%s %d]\r\n", __func__, __LINE__ );
}
void c2_network_cbk_delete(void *psHandle)
{
	skyfend_log_info( "[%s %d]\r\n", __func__, __LINE__ );
	c2_network_t *psNetwork = (c2_network_t*)psHandle;
	
	alink_disconnect_send( &psNetwork->sSendAlinkHeartBeat,			0xD0 );
	alink_disconnect_send( &psNetwork->sSendAlinkTargetInfo,		0x80);

	skyfend_free( psNetwork );
}

eth_link_cbk_t* c2_network_create( eth_link_user_t *psUser )
{
	c2_network_t *psNetwork = (c2_network_t*)skyfend_malloc(sizeof(c2_network_t));
	skyfend_log_info( "[%s %d]\r\n", __func__, __LINE__ );
	if( psNetwork )
	{
		memset( psNetwork, 0, sizeof(c2_network_t) );
		alink_connect_send( &psNetwork->sSendAlinkHeartBeat,		0xD0, ALINK_DEV_ID_C2, psUser, c2_network_send );
		alink_connect_send( &psNetwork->sSendAlinkTargetInfo,	0x80, ALINK_DEV_ID_C2, psUser, c2_network_send );
		
		alink_connect_port(&psNetwork->sAlinkSocket, NULL, ALINK_USER_TYPE_C2);
		
		psNetwork->psUser = psUser;
		psNetwork->sCbk.psHandle = psNetwork;
		psNetwork->sCbk.pv_cbk_connect		= c2_network_cbk_connect;
		psNetwork->sCbk.pv_cbk_data			= c2_network_cbk_data;
		psNetwork->sCbk.pv_cbk_disconnect	= c2_network_cbk_disconnect;
		psNetwork->sCbk.pv_cbk_delete		= c2_network_cbk_delete;
		return &(psNetwork->sCbk);
	}
	return NULL;
}

int32_t c2_network_init( void )
{
	return eth_link_add_type( 1, RTH_LINK_NET_TCP_CLIENT, 10, c2_network_create );
}
