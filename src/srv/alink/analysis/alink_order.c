
#include "alink_order.h"
#include "../alink_defines.h"
#include "../check/alink_check.h"
#include "../../log/skyfend_log.h"

static int32_t alink_order_send( alink_resp_t *psResp )
{
	int32_t eRet = 0;

	alink_msg_head_t *psHeaderSend;
	alink_msg_head_t *psHeader;
	alink_order_import_t *psImport;
	uint8_t *pbySend;
	uint16_t wCount;
	uint16_t wCrc;

	psImport = (alink_order_import_t *)(psResp->psArg);
	psHeader = (alink_msg_head_t*)(psImport->pbyMsg);
	
	if( psHeader->ans && psImport->pe_send )
	{
		psHeaderSend = (alink_msg_head_t*)(psResp->pbyBuffer - sizeof(alink_msg_head_t));
		psHeaderSend->magic		= ALINK_MSG_MAGIC_VALUE;
		psHeaderSend->len_lo	= (0xFF & (psResp->wCount >> 0));
		psHeaderSend->len_hi	= (0xFF & (psResp->wCount >> 8));
		psHeaderSend->seq		= psHeader->seq;
		psHeaderSend->destid	= psHeader->sourceid;
		psHeaderSend->sourceid	= psHeader->destid;
		psHeaderSend->msgid		= psHeader->msgid;
		psHeaderSend->ans		= 0;
		psHeaderSend->checksum	= alink_get_header_checksum(psHeaderSend);

		pbySend = (uint8_t*)(psHeaderSend);
		wCount = psResp->wCount + ALINK_NO_PAYLOAD_SIZE;
		wCrc = alink_get_package_crc( pbySend, wCount );
		pbySend[wCount - 2] = (0xFF & (wCrc >> 0));
		pbySend[wCount - 1] = (0xFF & (wCrc >> 8));

		skyfend_log_debug( "handle = %p\r\n", psImport->handle );
		eRet = psImport->pe_send( psImport->handle, pbySend, wCount );
	}

	return eRet;
}

int32_t alink_order_init( alink_order_t *psOrder, uint8_t *pbyBuffer, uint16_t wBufferSize )
{
	int32_t eRet = 0;

	if( 0 == pthread_mutex_init( &(psOrder->mutex), NULL ) )
	{
		psOrder->pbyBuffer		= pbyBuffer;
		psOrder->wBufferSize	= wBufferSize;
		psOrder->psCmdList		= NULL;
	}
	else
	{
		eRet = -1;
	}
	return eRet;
}

int32_t alink_order_task( alink_order_t *psOrder, alink_order_import_t *psImport )
{
	int32_t eRet = 0;
	alink_cmd_list_t *psCmdList;
	alink_msg_head_t *psHeader;
	alink_payload_t sPayload;
	alink_resp_t sResp;

	pthread_mutex_lock( &(psOrder->mutex) );
	
	psHeader = (alink_msg_head_t*)(psImport->pbyMsg);
	sResp.pbyBuffer		= (uint8_t*)psOrder->pbyBuffer + sizeof(alink_msg_head_t);
	sResp.wSize			= psOrder->wBufferSize - ALINK_NO_PAYLOAD_SIZE;
	sResp.pbyBuffer[0]	= 0xFF;
	sResp.wCount		= 1;
	sResp.pe_send		= alink_order_send;
	sResp.psArg			= psImport;

	for( psCmdList = psOrder->psCmdList; psCmdList; psCmdList = psCmdList->psNext )
	{
		if( psCmdList->byCmd == psHeader->msgid )
		{
			if( psCmdList->pv_cmd_task )
			{
				sPayload.ePortId	= psImport->ePortId;
				sPayload.eRemoteId	= psHeader->sourceid;
				sPayload.pbyData	= psImport->pbyMsg + sizeof(alink_msg_head_t);
				sPayload.wLength	= psHeader->len_hi << 8 | psHeader->len_lo;
				if( psCmdList->wPayloadLen <= sPayload.wLength )
					psCmdList->pv_cmd_task( &sPayload, &sResp );
			}
			break;
		}
	}
	
	// eRet = alink_order_send( &sResp );
	
	pthread_mutex_unlock( &(psOrder->mutex) );

	return eRet;
}

int32_t alink_order_register_cmd( alink_order_t *psOrder, alink_cmd_list_t *psCmdList )
{
	int32_t eRet = 0;
	alink_cmd_list_t *psCmdListNow;

	pthread_mutex_lock( &(psOrder->mutex) );

	psCmdListNow = psOrder->psCmdList;
	if( NULL == psCmdListNow )
	{
		psOrder->psCmdList = psCmdList;
	}
	else
	{	
		for( ; psCmdListNow != psCmdList; psCmdListNow = psCmdListNow->psNext )
		{
			if( NULL == psCmdListNow->psNext )
			{
				psCmdListNow->psNext = psCmdList;
				break;
			}
		}
	}
			
	pthread_mutex_unlock( &(psOrder->mutex) );

	return eRet;
}
