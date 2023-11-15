
#include <mqueue.h>
#include <sys/time.h>

#include "radar_network.h"
#include "../../srv/eth_link/eth_link.h"
#include "../../srv/alink/alink.h"
#include "../../srv/log/skyfend_log.h"
#include "../../common/tools/skyfend_tools.h"
#include "../../srv/alink/check/alink_check.h"

typedef struct radar_network
{
	alink_socket_t sAlinkSocket;

	eth_link_cbk_t	sCbk;
	eth_link_user_t *psUser;
}radar_network_t;

int32_t radar_network_send( void* handle, uint8_t *pbyData, uint32_t uLen )
{
	eth_link_user_t *psUser = (eth_link_user_t*)handle;
	return	psUser->pi_send( psUser->psSocket, pbyData, uLen );
}

static void radar_network_cbk_connect(void *psHandle)
{
	(void)psHandle;
	// skyfend_log_info( "[%s %d]", __func__, __LINE__ );
}

static void radar_network_cbk_data(void *psHandle,void *psClient,int32_t (*pi_recv)(void *psClient,void *pvBuffer,uint32_t uSize))
{
	radar_network_t *psNetwork = (radar_network_t*)psHandle;
	uint8_t abyBuffer[ALINK_HEADER_CHECKSUM_LEN] = {0};
	uint8_t *pabybuf = NULL;
	int32_t iLength = 0, rLenght = 0;
	int32_t integrity_pkg_len = 0;
	int32_t offset = 0;

	pabybuf = abyBuffer;
	rLenght = sizeof(abyBuffer);
	//fix receving whole package
	do {
		iLength = pi_recv( psClient, pabybuf + offset, rLenght);
		if(iLength <= 0) {// we take this status as overtime or error
			if(integrity_pkg_len == 0) {
				pabybuf = NULL;
			}
			goto cbk_exit;
		}
		if(integrity_pkg_len == 0) {
			integrity_pkg_len = alink_package_integrity_check(abyBuffer, iLength);
			if(integrity_pkg_len > 0) {
				pabybuf = (uint8_t *)malloc(integrity_pkg_len);
				if(!pabybuf) {
					skyfend_log_info("memory lack!!!\n");
					goto cbk_exit;
				}
				offset = alink_get_header_index(abyBuffer, iLength);
				memcpy(pabybuf, abyBuffer+offset, iLength-offset);
				iLength = iLength - offset;
				offset = 0;
			} else {//ignore none package head, depend on remote user.
				pabybuf = NULL;
				goto cbk_exit;
			}
		}

		offset += iLength;
		rLenght = integrity_pkg_len - offset;
		//APP_LOG_DEBUG("pkg len = %d, offset = %d, rlen = %d\n", integrity_pkg_len, offset, rLenght);
	}while(integrity_pkg_len > offset);

	alink_recv_import_t sImport;
	sImport.ePortId	= 1;
	sImport.pbyData	= pabybuf;
	sImport.uLength	= integrity_pkg_len;
	sImport.handle	= psNetwork->psUser->psSocket;
	sImport.pe_send	= psNetwork->psUser->pi_send;
	alink_socket_task( &psNetwork->sAlinkSocket, &sImport );

cbk_exit:
	if(pabybuf) {
		free(pabybuf);
	}
//	APP_LOG_DEBUG("%s,%d  exit!\n", __FUNCTION__, __LINE__);

	return;
}

static void radar_network_cbk_disconnect(void *psHandle)
{
	(void)psHandle;
	skyfend_log_info( "[%s %d]", __func__, __LINE__ );
}

void radar_network_cbk_delete(void *psHandle)
{
	skyfend_log_info( "[%s %d]", __func__, __LINE__ );
	radar_network_t *psNetwork = (radar_network_t*)psHandle;
	
	skyfend_free( psNetwork );
}

eth_link_cbk_t* radar_network_create( eth_link_user_t *psUser )
{
	radar_network_t *psNetwork = (radar_network_t*)skyfend_malloc(sizeof(radar_network_t));
	if( psNetwork )
	{
		memset( psNetwork, 0, sizeof(radar_network_t) );
		alink_connect_port(&psNetwork->sAlinkSocket, NULL, ALINK_USER_TYPE_DEVICE);
		
		psNetwork->psUser = psUser;
		psNetwork->sCbk.psHandle = psNetwork;
		psNetwork->sCbk.pv_cbk_connect		= radar_network_cbk_connect;
		psNetwork->sCbk.pv_cbk_data			= radar_network_cbk_data;
		psNetwork->sCbk.pv_cbk_disconnect	= radar_network_cbk_disconnect;
		psNetwork->sCbk.pv_cbk_delete		= radar_network_cbk_delete;
		return &(psNetwork->sCbk);
	}
	// skyfend_log_info( "[%s %d]", __func__, __LINE__ );

	return NULL;
}

int32_t radar_network_init(void)
{
	return eth_server_register(RTH_LINK_NET_TCP_CLIENT, radar_network_create);
}

int radar_network_release(void)
{
	return eth_link_server_uninit();
}
