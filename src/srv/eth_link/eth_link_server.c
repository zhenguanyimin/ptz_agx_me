
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
#include "device_network_manager.h"

#define DETECT_RADAR_DEVICE_TIME 2 //2 seconds
#define SERVER_TCP_TIMEOUT		 5//5 SECONDS
/*******************************************************************************
自动组网
*******************************************************************************/
typedef struct eth_link_list
{
	uint8_t				 net;
	uint8_t				 max; //the max connection
	uint16_t			 cur_con;//current conection number
	uint16_t			 type;
	uint16_t			 server_basical_port;
	uint32_t			 server_ip;
	pthread_mutex_t 	mutex;
	eth_link_cbk_t* 	 (*ps_init)(eth_link_user_t *psUser);
	void				 *list;
	struct eth_link_list *next;
}eth_link_list_t;
typedef struct eth_link_tcp_server_list
{
	tcp_server_cfg_t sCfg;
	tcp_server_cbk_t sCbk;
	tcp_server_t 	 sserver;
	eth_link_user_t	 sUser;
	eth_link_cbk_t	 *psUserCbk;
	eth_link_list_t	 *psRoot;
	struct eth_link_tcp_server_list *psNext;
}eth_link_tcp_server_list_t;

typedef struct detect_task_res {
	pthread_t 	detect_task_pid;
	bool 		exit_relase;
	int			detect_frequence; //uint: s
	udp_socket_t *sever_socket;
}detect_task_res_t;


static udp_socket_t sUdpSocket = { 0 };
static detect_task_res_t detect_task = {0};
static udp_socket_cfg_t sUdpCfg = 
{
	.bFlagRecv	= true,
	.bBroadcast	= true,
	.wRecvPort	= 1810,
	.uRecvIp	= INADDR_ANY,
	.wSendPort	= 1800,
	.uSendIp	= 0xFFFFFFFF,//0xFFFFFFFF,	//0xC0A8EBFF, //0xFFFFFFFF,
};

static eth_link_list_t *psEthList = NULL;

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

#ifdef NETWORK_DATA_DEBUG
	printf( "[%s %d] => %d", __func__, __LINE__, iLength );
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

static void *find_online_detect_task(void *p_arg)
{
	int plen = 0;
	eth_protocol_lookup_t ep_lookup_package = {0};
	detect_task_res_t *dt_obj = (detect_task_res_t *)p_arg;

	plen = eth_protocol_get_detect_package((void*)&ep_lookup_package, sizeof(eth_protocol_lookup_t));

#ifdef NETWORK_DATA_DEBUG
	char *ptmp = (char*)&ep_lookup_package;
	printf( "[%s %d]", __func__, __LINE__ );
	for( int i = 0; plen > i; ++i )
		printf( " %02X", ptmp[i] );
	printf( "\r\n" );
#endif

	dt_obj->exit_relase = false;
	while(!dt_obj->exit_relase) {
		NETWORK_DATA_DEBUG_PRINTF("detect radar one time per second....\n");
		udp_socket_send(dt_obj->sever_socket, (uint8_t *)&ep_lookup_package, plen);
		sleep(dt_obj->detect_frequence);
	}

	return NULL;
}

static udp_socket_cbk_t sUdpCbk = 
{
	.psHandle = NULL,
	.pv_cbk_init = eth_link_udp_cbk_init,
	.pv_cbk_data = eth_link_udp_cbk_data,
	.pv_cbk_delete = eth_link_udp_cbk_delete,
};

int eth_link_server_init(void)
{
	int32_t eRet = 0;
	
	eRet = udp_socket_crate(&sUdpSocket, &sUdpCfg, &sUdpCbk);
	if(0 != eRet) {
		return eRet;
	}
	detect_task.detect_frequence = DETECT_RADAR_DEVICE_TIME; // 2 seconds
	detect_task.sever_socket = &sUdpSocket;
	eRet = pthread_create(&detect_task.detect_task_pid, NULL, find_online_detect_task, &detect_task);
	if( 0 > eRet ) {
		NETWORK_DATA_DEBUG_PRINTF("create radar detection thread fail, ret = %d\n", eRet);
	}
	
	return eRet;
}

int eth_link_server_uninit(void)
{
	void *rval = NULL;

	udp_socket_close(&sUdpSocket);
	pthread_join(sUdpSocket.pid, &rval);
	detect_task.exit_relase = true;
	pthread_join(detect_task.detect_task_pid, &rval);

	for( eth_link_list_t *psList = psEthList; psList; psList = psList->next )
	{
		for(eth_link_tcp_server_list_t *ts_list = (eth_link_tcp_server_list_t *)psList->list; ts_list; ts_list=ts_list->psNext) {
			tcp_server_close(&ts_list->sserver);
			// pthread_join(ts_list->sserver.pid, &rval);
		}
	}

	return 0;
}

/*******************************************************************************
TCP服务端连接列
*******************************************************************************/
static void eth_link_del_tcp_server_list( eth_link_tcp_server_list_t *psList )
{
	eth_link_tcp_server_list_t *psIndex = (eth_link_tcp_server_list_t*)psList->psRoot->list;
	if( psList == psIndex )
	{
		psList->psRoot->list = psIndex->psNext;
	}
	else
	{
		eth_link_tcp_server_list_t *psLast = psIndex;
		for( ; psIndex; psIndex = psIndex->psNext )
		{
			if( psList == psIndex )
			{
				psLast->psNext = psIndex->psNext;
				break;
			}
		}
	}
	skyfend_free( psList );
}

static void eth_link_socket_cbk_connect( void *psHandle )
{
	eth_link_tcp_server_list_t *psList = (eth_link_tcp_server_list_t*)psHandle;
	if( psList && psList->psUserCbk && psList->psUserCbk->pv_cbk_connect )
		psList->psUserCbk->pv_cbk_connect( psList->psUserCbk->psHandle );
}

static void eth_link_socket_cbk_data( void *psHandle, void *psClient, int32_t (*pi_recv)(void *psClient,void *pvBuffer,uint32_t uSize) )
{
	eth_link_tcp_server_list_t *psList = (eth_link_tcp_server_list_t*)psHandle;
	if( psList && psList->psUserCbk && psList->psUserCbk->pv_cbk_data )
	{
		psList->psUserCbk->pv_cbk_data( psList->psUserCbk->psHandle, psClient, pi_recv );
	}
	else
	{
		uint8_t abyBuffer[128];
		int32_t iLength = pi_recv( psClient, abyBuffer, sizeof(abyBuffer) );
		(void)iLength;
	}
}

static void eth_link_socket_cbk_disconnect( void *psHandle )
{
	eth_link_tcp_server_list_t *psList = (eth_link_tcp_server_list_t*)psHandle;
	if( psList && psList->psUserCbk && psList->psUserCbk->pv_cbk_disconnect )
		psList->psUserCbk->pv_cbk_disconnect( psList->psUserCbk->psHandle );
}

static void eth_link_socket_cbk_delete( void *psHandle )
{
	eth_link_tcp_server_list_t *psList = (eth_link_tcp_server_list_t*)psHandle;
	if( psList && psList->psUserCbk && psList->psUserCbk->pv_cbk_delete ) {
		psList->psUserCbk->pv_cbk_delete( psList->psUserCbk->psHandle );
	}
	pthread_mutex_lock(&(psList->psRoot->mutex));
	eth_link_del_tcp_server_list( psList );
	pthread_mutex_unlock(&(psList->psRoot->mutex));
}

static void *eth_link_create_tcp_server_list( uint16_t wConnectTime, uint32_t uIp, uint16_t wPort, eth_link_cbk_t* (*ps_init)(eth_link_user_t *psUser) )
{
	eth_link_tcp_server_list_t *psList = (eth_link_tcp_server_list_t*)skyfend_malloc(sizeof(eth_link_tcp_server_list_t));
	int eRet = -1;

	memset( psList, 0, sizeof(eth_link_tcp_server_list_t) );

	NETWORK_DATA_DEBUG_PRINTF( "[%s %d] %x %d\r\n", __func__, __LINE__, uIp, wPort );
	if(psList) {
		psList->sCfg.ulocalIp		= uIp;
		psList->sCfg.wlocalPort	= wPort;
		psList->sCfg.wConnectTime	= wConnectTime;
		psList->sCbk.psHandle			= psList;
		psList->sCbk.pv_cbk_connect		= eth_link_socket_cbk_connect;
		psList->sCbk.pv_cbk_data		= eth_link_socket_cbk_data;
		psList->sCbk.pv_cbk_disconnect	= eth_link_socket_cbk_disconnect;
		psList->sCbk.pv_cbk_delete		= eth_link_socket_cbk_delete;

		NETWORK_DATA_DEBUG_PRINTF( "[%s %d]\r\n", __func__, __LINE__ );
		eRet = tcp_server_create( &(psList->sserver), &(psList->sCfg), &(psList->sCbk) );
		if(0 == eRet) {
			NETWORK_DATA_DEBUG_PRINTF( "[%s %d]\r\n", __func__, __LINE__ );
			psList->sUser.psSocket	= (void*)(&(psList->sserver));
			psList->sUser.pi_send	= (int32_t (*)(void*,uint8_t *,uint32_t))tcp_server_send;
			psList->sUser.pi_close	= (int32_t (*)(void*))tcp_server_close;
			psList->psUserCbk		= ps_init( &(psList->sUser) );
		} else {
			skyfend_free( psList );
			psList = NULL;
		}
	}

	return psList;
}

static uint16_t get_unused_port(eth_link_tcp_server_list_t *psList, uint16_t basical_port)
{
	eth_link_tcp_server_list_t *server_p = NULL;
	uint16_t port_sel = basical_port;

	for(int i = 0; i<100; i++) {
		for(server_p = psList; server_p; server_p = server_p->psNext) {
			if(server_p->sCfg.wlocalPort == port_sel) {
				break;
			}
		}
		if(!server_p) {
			break;
		}
		port_sel++;
	}

	return port_sel;
}

static int eth_link_add_tcp_server_list( eth_link_list_t* psEthList, char *sn, uint16_t wConnectTime )
{
	eth_link_tcp_server_list_t *psList = (eth_link_tcp_server_list_t*)psEthList->list;
	uint32_t uCount = 1;
	int ret = -1;

	if(psList) {

		/*find sn has been registered*/
		do
		{
			NETWORK_DATA_DEBUG_PRINTF("(%d)ssever.radar_sn = %s, client sn = %s\n", uCount, psList->sserver.radar_sn, sn);
			if(strncmp(psList->sserver.radar_sn, sn, RADAR_DEVICE_SN_LEN) == 0) {
				ret = -1;
				return ret;
			}
			if(psList->psNext) {
				psList = psList->psNext;
			} else {
				break;
			}
			uCount++;
		} while(1);

		/*check the init server in register interface: eth_server_register*/
		if(uCount == 1 && strncmp(psList->sserver.radar_sn, NETPOTOCAL_INITILA_SN, strlen(NETPOTOCAL_INITILA_SN)) == 0) {
			memcpy(psList->sserver.radar_sn, sn, RADAR_DEVICE_SN_LEN);
			ret = 0;
			NETWORK_DATA_DEBUG_PRINTF( "[%s %d], match init sn\r\n", __func__, __LINE__);
			return ret;
		}

		if(psEthList->max > uCount) {
			skyfend_log_info( "[%s %d], client sn: %s, server port = %d", __func__, __LINE__, sn, psEthList->server_basical_port + uCount);
			psEthList->cur_con = get_unused_port((eth_link_tcp_server_list_t*)psEthList->list, psEthList->server_basical_port);
			psList->psNext = eth_link_create_tcp_server_list(wConnectTime, psEthList->server_ip, \
															psEthList->cur_con,\
															psEthList->ps_init);
			if(psList->psNext) {
				psList = psList->psNext;
				psList->psNext = NULL;
				psList->psRoot = psEthList;
				memcpy(psList->sserver.radar_sn, sn, RADAR_DEVICE_SN_LEN);
				ret = 0;
			}
		} else {
			ret = -1;
		}
	} else {
		skyfend_log_info( "[%s %d], server ip:%d, init port is %d, sn:%s", __func__, __LINE__, \
						psEthList->server_ip, psEthList->server_basical_port, sn);
		psEthList->list = eth_link_create_tcp_server_list(wConnectTime, psEthList->server_ip, \
														 psEthList->server_basical_port, psEthList->ps_init);
		if( psEthList->list ) {
			psList = (eth_link_tcp_server_list_t*)psEthList->list;
			psList->psNext = NULL;
			psList->psRoot = psEthList;
			memcpy(psList->sserver.radar_sn, sn, RADAR_DEVICE_SN_LEN);
			ret = 0;
			psEthList->cur_con = psEthList->server_basical_port;
		}
	}

	return ret;
}

int eth_server_register(uint8_t net, eth_link_cbk_t* (*ps_init)(eth_link_user_t *psUser))
{
	int eRet = 0;
	unsigned char ip[64] = {0};
	
	if(!psEthList) {
		eth_link_list_t* list = (eth_link_list_t*)skyfend_malloc( sizeof(eth_link_list_t) );
		memset( list, 0, sizeof(eth_link_list_t) );
		NETWORK_DATA_DEBUG_PRINTF( "[%s %d]\r\n", __func__, __LINE__ );
		if( list ) {
			psEthList = (eth_link_list_t*)(list);
			psEthList->type		= 1;
			psEthList->net		= net;
			psEthList->max		= AGX_MAX_DEFAULT_NUM;
			psEthList->ps_init	= ps_init;
			psEthList->list		= NULL;
			psEthList->next		= NULL;
			psEthList->server_basical_port = AGX_DEFAULT_PORT_HEAD;
			if(agx_get_eht0_ip(ip)) {
				memcpy(ip, AGX_DEFAULT_IP, strlen((char*)ip));
			}
			psEthList->server_ip = inet_addr((char*)ip);
			if(0 != pthread_mutex_init( &(psEthList->mutex), NULL)) {
				skyfend_log_error("%s:%d > init eth thread mutex fail");
			}
		}
	}

	NETWORK_DATA_DEBUG_PRINTF( "[%s %d]\r\n", __func__, __LINE__ );
	eth_link_add_tcp_server_list(psEthList, NETPOTOCAL_INITILA_SN,  SERVER_TCP_TIMEOUT);

	return eRet;
}

/*******************************************************************************
创建连接
*******************************************************************************/
static int eth_link_create_server(uint16_t type, char *sn)
{
	int eRet = -1;

	for( eth_link_list_t *psList = psEthList; psList; psList = psList->next )
	{
		NETWORK_DATA_DEBUG_PRINTF("type: %d, psList->type = %d, psList->net = %d\n", type, psList->type, psList->net);
		switch( psList->net )
		{
			case RTH_LINK_NET_TCP_CLIENT:
				NETWORK_DATA_DEBUG_PRINTF( "[%s %d]\r\n", __func__, __LINE__ );
				pthread_mutex_lock(&psEthList->mutex);
				eRet = eth_link_add_tcp_server_list(psList, sn, SERVER_TCP_TIMEOUT);
				pthread_mutex_unlock(&psEthList->mutex);
				break;
			case RTH_LINK_NET_UDP:
				break;
			default:
				break;
		}
	}

	return eRet;
}

static int radar_connection_setup(eth_protocol_devinfo_body_t *psdev)
{
	return eth_link_create_server(psdev->type, (char *)psdev->sn);
}

static void eth_protocol_search_handler(eth_protocol_devinfo_body_t *psdev, uint32_t uIp)
{
	int ret = 0;
	eth_protocol_request_t ep_req_package = {0};
	eth_protocol_cntinfo_t ep_ctlinfo = {0};

#ifdef DEBUG_TRACK_INFO
	struct sockaddr_in	remoteAddr;
	remoteAddr.sin_addr.s_addr = htonl(uIp);
	skyfend_log_debug( "[%s %d], local:%d, remote:%d,%s\r\n", __func__, __LINE__, psEthList->server_ip, remoteAddr.sin_addr.s_addr, inet_ntoa(remoteAddr.sin_addr));
#endif

	if(htonl(uIp) == psEthList->server_ip) {
#ifdef NETWORK_DATA_DEBUG			
		skyfend_log_debug("[%s %d] > udp from local ip...", __func__, __LINE__);
#endif		
		return;
	}

	ret = radar_connection_setup(psdev);
	if(0 != ret) {
		return;
	}
	
	ep_ctlinfo.ip = psEthList->server_ip;//fix to first object ip.
	ep_ctlinfo.port = psEthList->cur_con;
	memcpy(ep_ctlinfo.sn, psdev->sn, strlen((char *)psdev->sn));
	ep_ctlinfo.type = 1;
	skyfend_log_info("%s:%d > ask device to connect server: ip =%d, port =%d, type=%d, sn=%s", __FUNCTION__, __LINE__, ep_ctlinfo.ip, ep_ctlinfo.port \
				, ep_ctlinfo.type,ep_ctlinfo.sn);

	ret = eth_protocol_get_request_package((void*)(&ep_req_package), &ep_ctlinfo);
	udp_socket_send(&sUdpSocket, (uint8_t *)&ep_req_package, ret);
	
	return;
}

static void eth_protocol_cfg_handler(eth_protocol_cntinfo_t *psCntInfo, uint32_t uIp)
{
	(void)psCntInfo;
	(void)uIp;
}

static void eth_protocol_handler( void* msg, uint32_t len, uint32_t uIp )
{
	uint8_t* pbyData = eth_protocol_analysis( msg, len );

	NETWORK_DATA_DEBUG_PRINTF("%s:%d>\n", __FUNCTION__, __LINE__);
	if(pbyData)
	{
		eth_protocol_head_t *psHead = (eth_protocol_head_t*)(msg);
		NETWORK_DATA_DEBUG_PRINTF("%s:%d> msgid = %02x\n", __FUNCTION__, __LINE__, psHead->msgid);
		switch( psHead->msgid )
		{
			case NETPROTOCAL_MSG_ID_LOOKUP_RESP:
				eth_protocol_search_handler((eth_protocol_devinfo_body_t*)pbyData, uIp);
				break;
			case NETPROTOCAL_MSG_ID_CONFIG_RESP:
				eth_protocol_cfg_handler((eth_protocol_cntinfo_t*)pbyData, uIp);
				break;
			default:
				break;
		}
	}
}
