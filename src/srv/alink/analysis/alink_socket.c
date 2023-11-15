

#include "alink_socket.h"
#include "alink_order.h"
#include "alink_recv.h"

void alink_socket_recv( alink_socket_t *psSocket, uint8_t *pbyData, uint32_t uLen )
{
	psSocket->sImport.pbyData	= pbyData;
	psSocket->sImport.uLength	= uLen;
	alink_recv_task( &(psSocket->sRecv), &(psSocket->sImport) );
}

int32_t alink_socket_init( alink_socket_t *psSocket, alink_order_t *psOrder )
{
	int32_t eRet = 0;
	
	alink_recv_init( &(psSocket->sRecv), psSocket->abyBuffer, 0 );
	eRet = alink_recv_register_cbk( &(psSocket->sRecv), psOrder, (int32_t (*)(void *psHandle, alink_order_import_t *psImport))alink_order_task );
	if( 0 == eRet )
	{
		psSocket->psOrder = psOrder;
	}
	else
	{
		eRet = -1;
	}
	
	return eRet;
}

void alink_socket_task( alink_socket_t *psSocket, alink_recv_import_t *psImport )
{
	memcpy( &psSocket->sImport, psImport, sizeof(alink_recv_import_t) );
	alink_recv_task( &(psSocket->sRecv), &(psSocket->sImport) );
}

int32_t alink_socket_connect( alink_socket_t *psSocket, alink_port_t *psPort )
{
	int32_t eRet = 0;

	psSocket->sImport.handle 	= psPort->handle;
	psSocket->sImport.pe_send	= psPort->pe_send;
	psSocket->sImport.ePortId	= psPort->byPortId;
	if( psPort->pv_register_cbk )
		psPort->pv_register_cbk( psSocket, (void (*)(void *psHandle, uint8_t *pbyData, uint32_t uLen))alink_socket_recv );

	return eRet;
}


int32_t alink_get_header_index(void *adat, uint32_t datlen)
{
	return alink_get_package_head_index(adat, datlen);
}

int32_t alink_package_integrity_check(void *adat, uint32_t datlen)
{
	return alink_package_valid_length(adat, datlen);
}
