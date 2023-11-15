
#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "sys/socket.h"
#include "sys/types.h"
#include "../../log/skyfend_log.h"

typedef struct tcp_server_cbk
{
	void		*psHandle;
	void		(*pv_cbk_connect)(void *psHandle);
	void		(*pv_cbk_data)(void	*psHandle,void *psClient,int32_t (*pi_recv)(void *psClient,void *pvBuffer,uint32_t uSize));
	void		(*pv_cbk_disconnect)(void *psHandle);
	void		(*pv_cbk_delete)(void *psHandle);
}tcp_server_cbk_t;

typedef struct tcp_server_cfg
{
	uint16_t	wConnectTime;
	uint16_t	wlocalPort;
	uint32_t	ulocalIp;
}tcp_server_cfg_t;

typedef struct tcp_server
{
	volatile bool		bFlagInit;
	volatile bool		bFlagConnect;
	volatile bool		bFlagClose;
	int32_t 			iSocket;
	pthread_t 			pid;
	pthread_mutex_t 	mutex;
	tcp_server_cfg_t	*psCfg;
	tcp_server_cbk_t	*psCbk;
	char				radar_sn[32];
	int				client_fd;	
}tcp_server_t;

int32_t tcp_server_create( tcp_server_t *psserver, tcp_server_cfg_t *psCfg, tcp_server_cbk_t *psCbk );

int32_t tcp_server_send( tcp_server_t *psserver, void *pvData, uint32_t uLength );

int32_t tcp_server_close( tcp_server_t *psserver );

#endif

