
#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

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

typedef struct tcp_client_cbk
{
	void		*psHandle;
	void		(*pv_cbk_connect)(void *psHandle);
	void		(*pv_cbk_data)(void	*psHandle,void *psClient,int32_t (*pi_recv)(void *psClient,void *pvBuffer,uint32_t uSize));
	void		(*pv_cbk_disconnect)(void *psHandle);
	void		(*pv_cbk_delete)(void *psHandle);
}tcp_client_cbk_t;

typedef struct tcp_client_cfg
{
	uint16_t	wConnectTime;
	uint16_t	wRemotePort;
	uint32_t	uRemoteIp;
}tcp_client_cfg_t;

typedef struct tcp_client
{
	volatile bool		bFlagInit;
	volatile bool		bFlagConnect;
	volatile bool		bFlagClose;
	int32_t 			iSocket;
	pthread_t 			pid;
	pthread_mutex_t 	mutex;
	tcp_client_cfg_t	*psCfg;
	tcp_client_cbk_t	*psCbk;
}tcp_client_t;

int32_t tcp_client_create( tcp_client_t *psClient, tcp_client_cfg_t *psCfg, tcp_client_cbk_t *psCbk );

int32_t tcp_client_send( tcp_client_t *psClient, void *pvData, uint32_t uLength );

int32_t tcp_client_close( tcp_client_t *psClient );

#endif

