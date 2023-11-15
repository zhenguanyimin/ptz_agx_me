
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <netinet/tcp.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>
#include "tcp_server.h"

static int32_t tcp_server_recv( void *p_arg, void* pvBuffer, uint32_t uSize )
{
	int32_t iLength = 0;
	
	tcp_server_t *psserver = (tcp_server_t*)p_arg;
	if( psserver && pvBuffer )
	{
		iLength = recv(psserver->client_fd, pvBuffer, uSize, 0);
		if( 0 >= iLength )
		{
			psserver->bFlagConnect = false;
			iLength = 0;
		} 	
#ifdef NETWORK_DATA_DEBUG
		else {
			printf( "[%s %d] => %d", __func__, __LINE__, iLength );
			char *ptm = (char*)pvBuffer;
			for( int i = 0; iLength > i; ++i )
				printf( " %02X", ptm[i] );
			printf( "\r\n" );
		}
#endif
	}
	
	return iLength;
}

/**
 * accept_timeout - 带超时的accept
 * @fd: 套接字
 * @addr: 输出参数，返回对方地址
 * @wait_seconds: 等待超时秒数，如果为0表示正常模式
 * 成功（未超时）返回已连接套接字，超时返回-1并且errno = ETIMEDOUT
 */
static int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
	int ret;
	long addrlen = sizeof(struct sockaddr_in);
	fd_set accept_fdset;
	struct timeval timeout;

	if(NULL == addr) {
		return -1;
	}

	if (wait_seconds > 0) {
		FD_ZERO(&accept_fdset);
		FD_SET(fd, &accept_fdset);
		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		do {
			ret = select(fd + 1, &accept_fdset, NULL, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);
		if (ret == -1) {
			skyfend_log_info("accpet select fail");
			return -1;
		} else if (ret == 0) {
			skyfend_log_info("accpet time out");
			errno = ETIMEDOUT;
			return -1;
		}
	}

	return accept(fd, (struct sockaddr*)addr, (uint32_t *) &addrlen); //返回已连接套接字
}

static void *tcp_server_task(void *p_arg)
{
	tcp_server_t *psserver	= (tcp_server_t*)p_arg;
	struct sockaddr_in	remoteAddr;
	struct sockaddr_in sLocalAddr;
	struct timeval timeout;
	fd_set		readSet;
	int32_t		iRet;
	int32_t		iLength;
	uint32_t	uConnectCount;
	// long lAddrLen = sizeof( struct sockaddr_in );
	int port_flag = 1;

	uConnectCount = 0;
	// remoteAddr.sin_family = AF_INET;
	// remoteAddr.sin_port = htons(psserver->psCfg->wRemotePort);
	// remoteAddr.sin_addr.s_addr = htonl(psserver->psCfg->uRemoteIp);

	NETWORK_DATA_DEBUG_PRINTF( "%s:%d > tcp_server_task  creaded, port = %d\r\n", __FUNCTION__, __LINE__, psserver->psCfg->wlocalPort);

	psserver->iSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(psserver->iSocket < 0) {
		return NULL;
	}
	memset((char *)&sLocalAddr, 0, sizeof(sLocalAddr));
	sLocalAddr.sin_family = AF_INET;
	//sLocalAddr.sin_len = sizeof(sLocalAddr);
	sLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sLocalAddr.sin_port = ntohs(((unsigned short)psserver->psCfg->wlocalPort));
	iRet = setsockopt(psserver->iSocket, SOL_SOCKET ,SO_REUSEADDR, &port_flag, sizeof(int));
	if(iRet < 0) {
		skyfend_log_warn("setsockopt SO_REUSEADDR fail");
	}
	iRet = bind(psserver->iSocket, (struct sockaddr*) &sLocalAddr, sizeof(sLocalAddr));
	if(iRet < 0) {
		close(psserver->iSocket);
		perror("bind fail");
		skyfend_log_fatal("bind fail");
		return NULL;
	}
	iRet = listen(psserver->iSocket, 20);
	if(iRet != 0) {
		close(psserver->iSocket);
		perror( "listen fail");
		skyfend_log_fatal("listen fail");
		return NULL;
	}
	
	NETWORK_DATA_DEBUG_PRINTF( "%s:%d > tcp_server_task  loop, port = %d\r\n", __FUNCTION__, __LINE__, psserver->psCfg->wlocalPort);

	for( ; ; ) {
		psserver->client_fd = accept_timeout(psserver->iSocket, &remoteAddr, 1);
		NETWORK_DATA_DEBUG_PRINTF( "%s:%d >tcp connect %d %s::%d\r\n", __FUNCTION__, __LINE__, psserver->client_fd, inet_ntoa(remoteAddr.sin_addr), psserver->psCfg->wlocalPort );
		if (psserver->client_fd < 0) {
			if( psserver->psCfg->wConnectTime )
			{
				if( psserver->psCfg->wConnectTime <= ++uConnectCount )
					break;
			}
			// close( psserver->iSocket );
			// psserver->iSocket = socket(AF_INET, SOCK_STREAM, 0);
			// sleep(1);
		} else {
			psserver->bFlagConnect	= true;
			uConnectCount			= 0;
			if(psserver->psCbk->pv_cbk_connect)
				psserver->psCbk->pv_cbk_connect(psserver->psCbk->psHandle);
			int keepalive = 1;
			int keepidle = 1;
			int keepinterval = 1;
			int keepcount = 1;
			struct timeval tv_timeout = {0};

			tv_timeout.tv_sec = 1;
			tv_timeout.tv_usec = 0;
			setsockopt(psserver->iSocket, SOL_SOCKET, SO_KEEPALIVE, &tv_timeout, sizeof(struct timeval));			
			setsockopt(psserver->iSocket, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));
			setsockopt(psserver->iSocket, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle));
			setsockopt(psserver->iSocket, IPPROTO_TCP, TCP_KEEPINTVL, &keepinterval, sizeof(keepinterval));
			setsockopt(psserver->iSocket, IPPROTO_TCP, TCP_KEEPCNT, &keepcount, sizeof(keepcount));
			for( ; ; )
			{
				/* waiting for data */
				FD_ZERO(&readSet);
				FD_SET(psserver->client_fd, &readSet);
				timeout.tv_sec			= 1;
				timeout.tv_usec			= 0;
				iRet = select(psserver->client_fd + 1, &readSet, NULL, NULL, &timeout);
				NETWORK_DATA_DEBUG_PRINTF( "%s:%d > (%s:%d)select ret %d\r\n", __FUNCTION__, __LINE__, inet_ntoa(remoteAddr.sin_addr), psserver->psCfg->wlocalPort, iRet);
				if( 0 < iRet )
				{
					uConnectCount = 0;
					// read the data
					if(FD_ISSET(psserver->client_fd, &readSet))
					{
						if( psserver->psCbk->pv_cbk_data )
						{
							psserver->psCbk->pv_cbk_data( psserver->psCbk->psHandle, psserver, tcp_server_recv );
							NETWORK_DATA_DEBUG_PRINTF( "%s:%d > pv_cbk_data back\r\n", __FUNCTION__, __LINE__);
						}
						else
						{
							uint8_t abyBuffer[128];
							iLength = tcp_server_recv( psserver, abyBuffer, sizeof(abyBuffer) );
							if( 0 >= iLength )
								break;
						}
						if( false == psserver->bFlagConnect )
							break;
					}
				} else {
					if(psserver->psCfg->wConnectTime <= ++uConnectCount) {
						break;
					}
					struct tcp_info info = {0};
            		int len = sizeof(struct tcp_info);
					getsockopt(psserver->iSocket, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
					if(info.tcpi_state == TCP_ESTABLISHED || info.tcpi_state == TCP_LISTEN) {
						skyfend_log_info("connect ok \r\n");
					}
					else {
						skyfend_log_info("connect error, %d\r\n", info.tcpi_state);
						break;
					}
				}
				if( psserver->bFlagClose )
					break;
			}
			NETWORK_DATA_DEBUG_PRINTF( "%s:%d > disconnect, reconnect\r\n", __FUNCTION__, __LINE__);
			// disconnect, reconnect
			psserver->bFlagConnect	= false;
			if( psserver->psCbk->pv_cbk_disconnect )
				psserver->psCbk->pv_cbk_disconnect( psserver->psCbk->psHandle );
			break;
		}
	}
	skyfend_log_notice("%s:%d exit, %s::%d", __FUNCTION__, __LINE__, inet_ntoa(remoteAddr.sin_addr), psserver->psCfg->wlocalPort);

	close(psserver->client_fd);
	psserver->client_fd = -1;
	if( 0 <= psserver->iSocket )
		close( psserver->iSocket );
	psserver->iSocket 	= -1;
	psserver->bFlagInit	= false;
	psserver->bFlagClose = false;
	pthread_mutex_destroy( &psserver->mutex );
	if( psserver->psCbk->pv_cbk_delete )
		psserver->psCbk->pv_cbk_delete( psserver->psCbk->psHandle );

	return 0;
}

int32_t tcp_server_create( tcp_server_t *psserver, tcp_server_cfg_t *psCfg, tcp_server_cbk_t *psCbk )
{
	int32_t eRet = 0;

	if( false == psserver->bFlagInit )
	{
		psserver->bFlagInit	= true;
		psserver->bFlagClose = false;
		psserver->bFlagConnect = false;
		if( psCfg && psCbk )
		{
			psserver->psCfg = psCfg;
			psserver->psCbk = psCbk;
			// psserver->iSocket = socket(AF_INET, SOCK_STREAM, 0);
			// if( 0 <= psserver->iSocket )
			// {
				// int keepalive = 1;
				// int keepidle = 1;
				// int keepinterval = 1;
				// int keepcount = 5;
				// setsockopt(psserver->iSocket, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));
				// setsockopt(psserver->iSocket, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle));
				// setsockopt(psserver->iSocket, IPPROTO_TCP, TCP_KEEPINTVL, &keepinterval, sizeof(keepinterval));
				// setsockopt(psserver->iSocket, IPPROTO_TCP, TCP_KEEPCNT, &keepcount, sizeof(keepcount));
				if( 0 == pthread_mutex_init( &(psserver->mutex), NULL ) )
				{
					if( 0 == pthread_create(&(psserver->pid), NULL, tcp_server_task, psserver) )
					{
						eRet = 0;
					}
					else
					{
						pthread_mutex_destroy( &psserver->mutex );
						// close( psserver->iSocket );
						psserver->bFlagInit	= false;
						eRet = -2;
					}
				}
				else
				{
					close( psserver->iSocket );
					psserver->bFlagInit	= false;
					eRet = -2;
				}
			// }
			// else
			// {
			// 	psserver->bFlagInit = false;
			// 	eRet = -2;
			// }
		}
		else
		{
			psserver->bFlagInit	= false;
			eRet = -1;
		}
	}

	return eRet;
}

int32_t tcp_server_send( tcp_server_t *psserver, void *pvData, uint32_t uLength )
{
	int32_t eRet = 0;
	
	if( psserver && pvData && psserver->bFlagConnect )
	{
		pthread_mutex_lock( &psserver->mutex );
		send( psserver->client_fd, pvData, uLength, 0);
		pthread_mutex_unlock( &psserver->mutex );
	}
	else
	{
		eRet = -2;
	}

	return eRet;
}

int32_t tcp_server_close( tcp_server_t *psserver )
{
	int32_t eRet = 0;

	if( psserver && psserver->bFlagConnect )
	{
		psserver->bFlagClose = true;
	}

	return eRet;
}
