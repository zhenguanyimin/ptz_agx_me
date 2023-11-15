
#include "alink_package.h"
#include "../check/alink_check.h"

int32_t alink_package_init( alink_package_list_t *psPackage, uint8_t byMsgId, uint8_t byAns, 
								uint16_t (*pw_package_task)(uint8_t* pbyBuffer, uint16_t wSize, void *psImport) )
{
	int32_t eRet = 0;

	psPackage->byAns 			= byAns;
	psPackage->byMsgId 			= byMsgId;
	psPackage->pw_package_task	= pw_package_task;
	psPackage->psNext			= NULL;

	return eRet;
}

int32_t alink_package_connect_send( alink_package_list_t *psPackage, alink_send_list_t *psSendList )
{
	int32_t eRet = 0;
	alink_send_list_t *psSendListNow;

	psSendListNow = psPackage->psSendList;
	if( NULL == psSendListNow )
	{
		psPackage->psSendList = psSendList;
	}
	else
	{
		for( ; psSendListNow != psSendList; psSendListNow = psSendListNow->psNext )
		{
			if( NULL == psSendListNow->psNext )
			{
				psSendListNow->psNext = psSendList;
				break;
			}
		}
	}

	return eRet;
}

int32_t alink_package_disconnect_send( alink_package_list_t *psPackage, alink_send_list_t *psSendList )
{
	int32_t eRet = 0;
	alink_send_list_t *psSendListLast = NULL;

	for( alink_send_list_t *psSendListNow = psPackage->psSendList; psSendListNow; psSendListNow = psSendListNow->psNext )
	{
		if( psSendListNow == psSendList )
		{
			if( psSendListLast )
				psSendListLast->psNext = psSendListNow->psNext;
			else
				psPackage->psSendList = NULL;
			break;
		}
		else
		{
			psSendListLast = psSendListNow;
		}
	}

	return eRet;
}

