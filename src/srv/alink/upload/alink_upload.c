
#include "alink_upload.h"
#include "alink_package.h"
#include "../check/alink_check.h"
#include "../../../srv/log/skyfend_log.h"

int32_t alink_upload_init( alink_upload_t *psUpload, uint8_t byDevId, uint8_t *pbyBuffer, uint16_t wBufferSize )
{
	int32_t eRet = 0;

	if( 0 == pthread_mutex_init( &(psUpload->mutex), NULL ) )
	{
		psUpload->byReq				= 0;
		psUpload->byDevId			= byDevId;
		psUpload->pbyBuffer			= pbyBuffer;
		psUpload->wBufferSize		= wBufferSize;
	}
	else
	{
		eRet = -1;
	}

	return eRet;
}

int32_t alink_upload_send_package( alink_upload_t *psUpload, alink_package_list_t *psPackage, void *psImport )
{
	int32_t eRet = 0;
	uint16_t wPayloadLen;
	uint16_t wCount;
	uint16_t wCrcCalc;
	uint8_t *pbyBuffer;
	alink_msg_head_t *psHeader;
	alink_send_list_t *psSendList;

	if( psPackage->psSendList && psPackage->pw_package_task )
	{
		pbyBuffer = psUpload->pbyBuffer;
		psHeader = (alink_msg_head_t*)pbyBuffer;

		pthread_mutex_lock( &psUpload->mutex );
		
		wPayloadLen = psPackage->pw_package_task( (uint8_t*)psUpload->pbyBuffer + sizeof(alink_msg_head_t), psUpload->wBufferSize - ALINK_NO_PAYLOAD_SIZE, psImport );

		psHeader->magic = ALINK_MSG_MAGIC_VALUE;
		psHeader->len_lo = (wPayloadLen>>0) & 0xFF;
		psHeader->len_hi = (wPayloadLen>>8) & 0xFF;
		psHeader->sourceid = psUpload->byDevId;
		psHeader->seq = psUpload->byReq++;
		psHeader->msgid = psPackage->byMsgId;
		psHeader->ans = psPackage->byAns;

		for( psSendList = psPackage->psSendList; psSendList; psSendList = psSendList->psNext )
		{
			if( psSendList->pe_send )
			{
				psHeader->destid = psSendList->byRemoteId;
				psHeader->checksum = alink_get_header_checksum( psHeader );
				wCount = wPayloadLen + ALINK_NO_PAYLOAD_SIZE;
				wCrcCalc = alink_get_package_crc( (uint8_t*)pbyBuffer, wCount );
				pbyBuffer[wCount - 2] = 0xFF & (wCrcCalc >> 0);
				pbyBuffer[wCount - 1] = 0xFF & (wCrcCalc >> 8);
				eRet = psSendList->pe_send( psSendList->handle, pbyBuffer, wCount );
			}
		}
		
		pthread_mutex_unlock( &psUpload->mutex );
	}

	return eRet;
}

int32_t alink_upload_register_package(         alink_upload_t *psUpload, alink_package_list_t *psPackageList )
{
	int32_t eRet = 0;
	alink_package_list_t *psPackageListNow;

	pthread_mutex_lock( &(psUpload->mutex) );

	psPackageListNow = psUpload->psPackageList;
	if( NULL == psPackageListNow )
	{
		psUpload->psPackageList = psPackageList;
	}
	else
	{	
		for( ; psPackageListNow != psPackageList; psPackageListNow = psPackageListNow->psNext )
		{
			if( NULL == psPackageListNow->psNext )
			{
				psPackageListNow->psNext = psPackageList;
				break;
			}
		}
	}
			
	pthread_mutex_unlock( &(psUpload->mutex) );

	return eRet;

}

int32_t alink_upload_connect_send( alink_upload_t *psUpload, uint8_t byMsgId, alink_send_list_t *psSendList )
{
	int32_t eRet = -1;

	pthread_mutex_lock( &psUpload->mutex );
	
	for( alink_package_list_t *psPackageListNow = psUpload->psPackageList; psPackageListNow; psPackageListNow = psPackageListNow->psNext )
	{
		if( psPackageListNow->byMsgId == byMsgId )
		{
			eRet = alink_package_connect_send( psPackageListNow, psSendList );
			break;
		}
	}

	pthread_mutex_unlock( &psUpload->mutex );

	return eRet;
}

int32_t alink_upload_disconnect_send( alink_upload_t *psUpload,         uint8_t byMsgId, alink_send_list_t *psSendList )
{
	int32_t eRet = -1;

	pthread_mutex_lock( &psUpload->mutex );
	
	for( alink_package_list_t *psPackageListNow = psUpload->psPackageList; psPackageListNow; psPackageListNow = psPackageListNow->psNext )
	{
		if( psPackageListNow->byMsgId == byMsgId )
		{
			eRet = alink_package_disconnect_send( psPackageListNow, psSendList );
			break;
		}
	}

	pthread_mutex_unlock( &psUpload->mutex );

	return eRet;
}


