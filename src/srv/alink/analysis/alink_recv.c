
#include "alink_recv.h"
#include "alink_order.h"
#include "../check/alink_check.h"

static uint32_t alink_recv_state_check_magic( alink_recv_t *psRecv, alink_recv_import_t *psImport );
static uint32_t alink_recv_state_check_head( alink_recv_t *psRecv, alink_recv_import_t *psImport );
static uint32_t alink_recv_state_check_crc( alink_recv_t *psRecv, alink_recv_import_t *psImport );

static uint32_t alink_recv_state_check_magic( alink_recv_t *psRecv, alink_recv_import_t *psImport )
{
	uint32_t uIndex;

	for( uIndex = 0; psImport->uLength > uIndex; ++uIndex )
	{
		if( alink_check_magic(psImport->pbyData[uIndex]) )
		{
			psRecv->pbyBuffer = psImport->pbyData+uIndex;
			psRecv->wCount = 1;
			psRecv->pu_state = alink_recv_state_check_head;
			++uIndex;
			break;
		}
	}
	return uIndex;
}

static uint32_t alink_recv_state_check_head( alink_recv_t *psRecv, alink_recv_import_t *psImport )
{
	uint32_t uIndex;

	if( sizeof(alink_msg_head_t) - psRecv->wCount > psImport->uLength )
	{
		uIndex = psImport->uLength;
		psRecv->wCount += psImport->uLength;
	}
	else
	{
		uIndex = sizeof(alink_msg_head_t) - psRecv->wCount;
		psRecv->wCount += uIndex;
		if( alink_check_header_checksum( (alink_msg_head_t*)psRecv->pbyBuffer ) )
		{
			psRecv->wPkgSize = alink_get_package_size( (alink_msg_head_t*)psRecv->pbyBuffer );
			psRecv->pu_state = alink_recv_state_check_crc;
		}
		else
		{
			psRecv->pu_state = alink_recv_state_check_magic;
		}
	}

	return uIndex;
}

static uint32_t alink_recv_state_check_crc( alink_recv_t *psRecv, alink_recv_import_t *psImport )
{
	uint32_t uIndex;
	alink_order_import_t sOrderImport;
	uint32_t uLen = psImport->uLength;

	if( psRecv->wPkgSize > psRecv->wCount + uLen )
	{
		uIndex = uLen;
		psRecv->wCount += uLen;
	}
	else
	{
		uIndex = psRecv->wPkgSize - psRecv->wCount;
		psRecv->wCount += uIndex;
		if( alink_check_crc( psRecv->pbyBuffer, psRecv->wCount ) )
		{
			if( psRecv->sRecvCbk.pe_cbk )
			{
				sOrderImport.ePortId = psImport->ePortId;
				sOrderImport.handle	 = psImport->handle;
				sOrderImport.pe_send = psImport->pe_send;
				sOrderImport.pbyMsg	 = (uint8_t*)psRecv->pbyBuffer;
				psRecv->sRecvCbk.pe_cbk( psRecv->sRecvCbk.psHandle, &sOrderImport );
			}
		}
		psRecv->pu_state = alink_recv_state_check_magic;
	}

	return uIndex;
}

int32_t alink_recv_init( alink_recv_t *psRecv, uint8_t *pbyBuffer, uint16_t wBufferSize )
{
	int32_t eRet = 0;

	psRecv->pbyBuffer			= pbyBuffer;
	psRecv->wBufferSize			= wBufferSize;
	psRecv->sRecvCbk.psHandle	= NULL;
	psRecv->sRecvCbk.pe_cbk		= NULL;
	psRecv->pu_state			= alink_recv_state_check_magic;

	return eRet;
}

void alink_recv_task( alink_recv_t *psRecv, alink_recv_import_t *psImport )
{
	alink_recv_import_t sImport;

	memcpy( &sImport, psImport, sizeof(sImport) );
	for( uint32_t uIndex = 0; sImport.uLength; sImport.uLength -= uIndex )
	{
		uIndex = psRecv->pu_state( psRecv, &sImport );
		sImport.pbyData += uIndex;
	}
}

uint32_t alink_package_valid_length(void *psRecvdat, uint32_t recvlen)
{
	uint32_t uIndex = 0;
	uint32_t upkglen = 0;
	char *pr = (char*)psRecvdat;

	for( uIndex = 0; recvlen > uIndex; ++uIndex )
	{
		if( alink_check_magic(pr[uIndex]) )
		{
			upkglen = alink_get_package_size( (alink_msg_head_t*)(pr+uIndex) );
			break;
		}
	}

	return upkglen;
}

uint32_t alink_get_package_head_index(void *psRecvdat, uint32_t recvlen)
{
	uint32_t uIndex = 0;
	char *pr = (char*)psRecvdat;

	for( uIndex = 0; recvlen > uIndex; ++uIndex )
	{
		if( alink_check_magic(pr[uIndex]) )
		{
			break;
		}
	}

	return uIndex;
}

int32_t alink_recv_register_cbk( alink_recv_t *psRecv, void *psHandle, int32_t (*pe_cbk)(void *psHandle, alink_order_import_t *psImport) )
{
	int32_t eRet = 0;

	psRecv->sRecvCbk.psHandle	= psHandle;
	psRecv->sRecvCbk.pe_cbk		= pe_cbk;

	return eRet;
}
