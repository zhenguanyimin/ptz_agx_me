
#include "alink_check.h"
#include "checksum.h"
#include "../alink_defines.h"

uint8_t alink_get_header_size( void )
{
	return (uint8_t)sizeof(alink_msg_head_t);
}

uint16_t alink_get_payload_size( alink_msg_head_t *psHeader )
{
	uint16_t wSize = 0;

	wSize = psHeader->len_hi;
	wSize <<= 8;
	wSize |= psHeader->len_lo;
	return wSize;
}

uint32_t alink_get_package_size( alink_msg_head_t *psHeader )
{
	uint32_t wSize = 0;

	wSize = alink_get_payload_size( psHeader );
#ifdef ALINK_CHECK_PIL_SPECAIL
	if(wSize == 0xffff) {
		wSize = 512*1024 + 128 + 7;
	}
#endif

	wSize += ALINK_NO_PAYLOAD_SIZE;

	return wSize;
}

uint8_t alink_get_header_checksum( alink_msg_head_t *psHeader )
{
	uint8_t checksum = 0;
	uint8_t i = 0;

	for (i = 0; ALINK_HEADER_CHECKSUM_LEN > i; i++)
	{
		checksum += ((uint8_t*)psHeader)[i];
	}
	return checksum;
}

uint16_t alink_get_package_crc( uint8_t *pbyPkg, uint16_t wPkgSize )
{
	uint16_t wCrcCalc;
	wCrcCalc = crc_calculate( (const uint8_t*)pbyPkg + 1, wPkgSize - 3 );
	return wCrcCalc;
}

bool alink_check_magic( uint8_t x )
{
	return (bool)(ALINK_MSG_MAGIC_VALUE == x);
}

bool alink_check_header_checksum( alink_msg_head_t *psHeader )
{
	bool bRet = true;
	uint8_t checksum;

	checksum = alink_get_header_checksum( psHeader );
	if( psHeader->checksum != checksum )
	{
		bRet = false;
		printf("[%s,%d] 0x%x!=0x%x\r\n", __FUNCTION__, __LINE__, psHeader->checksum, checksum);
	}
	return bRet;
}

bool alink_check_crc( uint8_t *pbyPkg, uint32_t wLen )
{
	bool bRet = false;
	uint32_t wPgkLen;
	uint16_t wCrcCalc;
	uint16_t wCrcRecv;
	uint8_t* pbyCrcIndex;

	wPgkLen = alink_get_package_size( (alink_msg_head_t*)pbyPkg );
	if( wPgkLen == wLen )
	{
		wCrcCalc = crc_calculate( (const uint8_t*)pbyPkg + 1, wPgkLen - 3 );
		pbyCrcIndex = (uint8_t*)pbyPkg + wPgkLen - 2;
		wCrcRecv = pbyCrcIndex[1];
		wCrcRecv <<= 8;
		wCrcRecv |= pbyCrcIndex[0];
#ifdef ALINK_CHECK_PIL_SPECAIL
		if( wCrcRecv == wCrcCalc ||(((alink_msg_head_t*)pbyPkg)->msgid == 0x22 ))
#else
		if( wCrcRecv == wCrcCalc )
#endif
		{
			bRet = true;
		}
		else
		{
			printf("[%s:%d] wCrcCalc=0x%x isn't same as wCrcRecv=0x%x\r\n", __FUNCTION__, __LINE__, wCrcCalc, wCrcRecv);
		}
	}
	return bRet;
}

bool alink_check_package( uint8_t *pbyPkg, uint16_t wLen )
{
	bool bRet;

	bRet = alink_check_magic( pbyPkg[0] );
	if( false == bRet )
	{
		printf("[%s,%d] 0x%x!=0x%x\r\n", __FUNCTION__, __LINE__, pbyPkg[0], ALINK_MSG_MAGIC_VALUE);
		goto out;
	}

	if( ALINK_NO_PAYLOAD_SIZE > wLen )
	{
		bRet = false;
		goto out;
	}

	bRet = alink_check_header_checksum( (alink_msg_head_t*)pbyPkg );
	if( false == bRet )
	{
		goto out;
	}

	bRet = alink_check_crc( pbyPkg, wLen );
	if( false == bRet )
	{
		goto out;
	}

out:
	return bRet;
}
