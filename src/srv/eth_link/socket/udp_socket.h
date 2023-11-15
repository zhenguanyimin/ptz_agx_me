
#ifndef _UDP_SOCKET_H_
#define _UDP_SOCKET_H_

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

typedef struct udp_socket_cfg
{
	bool		bFlagRecv;
	bool		bBroadcast;
	uint16_t	wRecvPort;
	uint16_t	wSendPort;
	uint32_t	uRecvIp;
	uint32_t	uSendIp;
}udp_socket_cfg_t;

typedef struct udp_socket_cbk
{
	void		*psHandle;
	void		(*pv_cbk_init)(void	*psHandle,void *psSocket );
	void		(*pv_cbk_data)(void	*psHandle,int32_t (*pi_recv)(void *psSocket,void *pvBuffer,uint32_t uSize,uint32_t *puIp));
	void		(*pv_cbk_delete)(void *psHandle);
}udp_socket_cbk_t;

typedef struct udp_socket
{
	volatile bool		bFlagInit;
	volatile bool		bFlagClose;
	int32_t 			iSocket;
	pthread_t 			pid;
	pthread_mutex_t 	mutex;
	udp_socket_cfg_t	*psCfg;
	udp_socket_cbk_t	*psCbk;
}udp_socket_t;

int32_t udp_socket_crate( udp_socket_t *psSocket, udp_socket_cfg_t *psCfg, udp_socket_cbk_t *psCbk );

int32_t udp_socket_close( udp_socket_t *psSocket );

int32_t udp_socket_send( udp_socket_t *psSocket, uint8_t *pbyData, uint32_t uLength );

#endif

