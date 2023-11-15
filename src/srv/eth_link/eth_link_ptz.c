
#include "eth_link.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <fcntl.h>

#include "socket/udp_socket.h"
#include "socket/tcp_server.h"
#include "../alink/alink.h"
#include "eth_protocol/netcon_protocol.h"
#include "../log/skyfend_log.h"
#include "../../common/tools/skyfend_tools.h"
#include "eth_link_ptz.h"

typedef struct eth_link_ptz_list
{
	msg_handle_info_t msdhi;
	struct eth_link_ptz_list *next;
}eth_link_ptz_list_t;

static udp_socket_cfg_t sUdpCfg = 
{
	.bFlagRecv	= true,
	.bBroadcast	= true,
	.wRecvPort	= 9966,
	.uRecvIp	= INADDR_ANY,
	.wSendPort	= 9966,
	.uSendIp	= 0xFFFFFFFF,//0xFFFFFFFF,	//0xC0A8EBFF, //0xFFFFFFFF,
};

static udp_socket_t sUdpSocket = { 0 };
static eth_link_ptz_list_t *ptz_list_obj = NULL;

static void eth_link_udp_cbk_init( void* psHandle, void* result )
{
	NETWORK_DATA_DEBUG_PRINTF( "[%s %d] %p\r\n", __func__, __LINE__, result );
}

static void eth_protocol_handler( void* msg, uint32_t len, uint32_t uIp );
static void eth_link_udp_cbk_data( void	*psHandle, int32_t (*pi_recv)(void *psSocket,void *pvBuffer,uint32_t uSize,uint32_t *puIp) )
{
	uint8_t abyBuffer[512] = {0};
	int32_t iLength;
	uint32_t uIp = 0xFFFFFFFF;
	
	iLength = pi_recv( &sUdpSocket, abyBuffer, sizeof(abyBuffer), &uIp );

	sUdpCfg.uSendIp = uIp | 0xFF;

#if 1
	skyfend_log_debug( "[%s %d] => %d", __func__, __LINE__, iLength );
	for( int i = 0; iLength > i; ++i )
		printf( " %02X", abyBuffer[i] );
	printf( "\r\n" );
#endif

	eth_protocol_handler( abyBuffer, iLength, uIp );

	sUdpCfg.uSendIp = 0xFFFFFFFF;
}

static void eth_link_udp_cbk_delete( void *psHandle )
{
	NETWORK_DATA_DEBUG_PRINTF( "[%s %d]\r\n", __func__, __LINE__ );
}

static udp_socket_cbk_t sUdpCbk = 
{
	.psHandle = NULL,
	.pv_cbk_init = eth_link_udp_cbk_init,
	.pv_cbk_data = eth_link_udp_cbk_data,
	.pv_cbk_delete = eth_link_udp_cbk_delete,
};

int eth_link_ptz_init(void)
{
	return udp_socket_crate(&sUdpSocket, &sUdpCfg, &sUdpCbk);
}

int eth_link_ptz_uninit(void)
{
	eth_link_ptz_list_t *pos = ptz_list_obj;
	eth_link_ptz_list_t *tmp = NULL;
	void *rval = NULL;

	udp_socket_close(&sUdpSocket);
	pthread_join(sUdpSocket.pid, &rval);
	for(pos = ptz_list_obj; pos;) { 
		tmp = pos;
		pos = pos->next;
		skyfend_free(tmp);
	}
	ptz_list_obj = NULL;

	return 0;
}

int eth_ptz_command_register(uint32_t msgid, ptz_comand_handle cb)
{
	eth_link_ptz_list_t *pos = ptz_list_obj;

	if(!pos) {
		ptz_list_obj = skyfend_malloc(sizeof(eth_link_ptz_list_t));
		if(!ptz_list_obj) {
			skyfend_log_error("%s:%d> memory error...\n", __FUNCTION__, __LINE__);
			return -1;
		}
		ptz_list_obj->msdhi.msg_id = msgid;
		ptz_list_obj->msdhi.cbk = cb;
		ptz_list_obj->next = NULL;
	} else {
		while(pos->next != NULL) {
			pos = pos->next;
		}
		pos->next = skyfend_malloc(sizeof(eth_link_ptz_list_t));
		if(!pos->next) {
			skyfend_log_error("%s:%d> memory error...\n", __FUNCTION__, __LINE__);
			return -1;
		}
		pos->next->msdhi.msg_id = msgid;
		pos->next->msdhi.cbk = cb;
		pos->next->next = NULL;
	}

	return 0;
}

int send_ptz_package(ptz_protocol_msg_t *msginfo)
{
	int retlen = 0;
	uint8_t pac[PTZ_PACKAGE_MAX_LEN] = {0};

	retlen = ptz_protocol_package(msginfo->command, msginfo->msg, msginfo->msglen, pac);

	udp_socket_send(&sUdpSocket, pac, retlen);

	return 0;
}

static void eth_protocol_handler( void* msg, uint32_t len, uint32_t uIp )
{
	int ret = -1;
	ptz_protocol_msg_t lmsg = {0};
	eth_link_ptz_list_t *pos = ptz_list_obj;

	ret = ptz_protocol_network_data_analysis(msg, len, &lmsg);
	if(0 != ret) {
		ptz_protocol_msg_destory(&lmsg);
		skyfend_log_error("package parsed fail\n");
		return;
	}

	for(pos = ptz_list_obj; pos; pos = pos->next) {
		// APP_LOG_DEBUG("register msgid %x, pac id %x\n", pos->msdhi.msg_id, lmsg.command);
		if(pos->msdhi.msg_id == lmsg.command) {
			if(pos->msdhi.cbk) {
				pos->msdhi.cbk(&lmsg);
				break;
			}
		}
	}
	ptz_protocol_msg_destory(&lmsg);
}
