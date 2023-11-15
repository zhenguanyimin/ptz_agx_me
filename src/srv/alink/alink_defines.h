
#ifndef _ALINK_DEFINES_H_
#define _ALINK_DEFINES_H_

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include <pthread.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define ALINK_USER_TYPE_C2		0  //default is c2
#define ALINK_USER_TYPE_DEVICE  1

#define ALINK_RECV_BUFFER_SIZE		(10*1024)
#define ALINK_SEND_BUFFER_SIZE		(16*1024)
#define ALINK_UPLOAD_BUFFER_SIZE	(6*1024)

#pragma pack(1)
	typedef struct alink_msg_head
	{
		uint8_t magic;    ///< Protocol magic marker
		uint8_t len_lo;   ///< Low byte of length of payload
		uint8_t len_hi;   ///< High byte of length of payload
		uint8_t seq;   	  ///< packet sequence
		uint8_t destid;   ///< ID of message destination
		uint8_t sourceid; ///< ID of the message sounrce
		uint8_t msgid;    ///< ID of message in payload
		uint8_t ans;
		uint8_t checksum; ///< Checksum of message header
	} alink_msg_head_t;
#pragma pack()

typedef struct alink_payload
{
	uint8_t		ePortId;
	uint8_t 	eRemoteId;
	uint32_t	wLength;
	uint8_t*	pbyData;
}alink_payload_t;

typedef struct alink_resp
{
	uint32_t	wCount;
	uint32_t 	wSize;
	uint8_t		*pbyBuffer;
	int32_t		(*pe_send)(struct alink_resp *psResp);
	void		*psArg;
}alink_resp_t;

typedef struct alink_cmd_list
{
	uint8_t					byCmd;
	uint16_t				wPayloadLen;
	void					(*pv_cmd_task)(alink_payload_t *psPayload, alink_resp_t *psResp);
	struct alink_cmd_list	*psNext;
	uint8_t					cmd_type;
}alink_cmd_list_t;

typedef struct alink_order_import
{
	uint8_t 	ePortId;
	uint8_t		*pbyMsg;
	void		*handle;
	int32_t		(*pe_send)(void *handle, uint8_t *pbyData, uint32_t uLength);
}alink_order_import_t;

typedef struct alink_order
{
	uint32_t			wBufferSize;
	pthread_mutex_t 	mutex;
	uint8_t				*pbyBuffer;
	alink_cmd_list_t	*psCmdList;
}alink_order_t;

typedef struct alink_recv_import
{
	uint8_t 	ePortId;
	uint8_t 	*pbyData;
	uint32_t 	uLength;
	void		*handle;
	int32_t		(*pe_send)(void* handle, uint8_t *pbyData, uint32_t uLength);
}alink_recv_import_t;

typedef struct alink_recv_cbk
{
	void		*psHandle;
	int32_t 	(*pe_cbk)( void *psHandle, alink_order_import_t *psImport );
}alink_recv_cbk_t;

typedef struct alink_recv
{
	volatile uint32_t	wPkgSize;
	volatile uint32_t 	wCount;
	uint32_t			wBufferSize;
	alink_recv_cbk_t	sRecvCbk;
	uint8_t 			*pbyBuffer;
	uint32_t			(*pu_state)( struct alink_recv *psRecv, alink_recv_import_t *psImport );
}alink_recv_t;

typedef struct alink_socket
{
	uint8_t				*abyBuffer;
	alink_recv_t		sRecv;
	alink_recv_import_t sImport;
	alink_order_t		*psOrder;
}alink_socket_t;

typedef struct alink_port
{
	uint8_t	byPortId;
	void	*handle;
	int32_t	(*pe_send)(void* handle, uint8_t *pbyData, uint32_t uLen);
	void 	(*pv_register_cbk)( void *psHandle, void (*pv_cbk)(void *psHandle, uint8_t *pbyData, uint32_t uLen));
}alink_port_t;

typedef struct alink_send_list
{
	uint8_t					byRemoteId;
	void					*handle;
	int32_t					(*pe_send)(void* handle, uint8_t *pbyData, uint32_t uLen);
	struct alink_send_list	*psNext;
}alink_send_list_t;

typedef struct alink_package_list
{
	uint8_t						byMsgId;
	uint8_t						byAns;
	uint16_t 					(*pw_package_task)(uint8_t* pbyBuffer, uint16_t wSize, void *psImport);
	alink_send_list_t 			*psSendList;
	struct alink_package_list	*psNext;
}alink_package_list_t;

typedef struct alink_upload
{
	uint8_t					byDevId;
	uint8_t					byReq;
	uint32_t				wBufferSize;
	pthread_mutex_t 		mutex;
	uint8_t					*pbyBuffer;
	alink_package_list_t	*psPackageList;
}alink_upload_t;

#ifdef __cplusplus
}
#endif

#endif
