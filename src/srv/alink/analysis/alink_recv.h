
#ifndef ALINK_RECV_H
#define ALINK_RECV_H

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "../alink_defines.h"

int32_t alink_recv_init( alink_recv_t *psRecv, uint8_t *pbyBuffer, uint16_t wBufferSize );

void alink_recv_task( alink_recv_t *psRecv, alink_recv_import_t *psImport );

int32_t alink_recv_register_cbk( alink_recv_t *psRecv, void *psHandle, int32_t (*pe_cbk)(void *psHandle, alink_order_import_t *psImport) );

uint32_t alink_get_package_head_index(void *psRecvdat, uint32_t recvlen);

uint32_t alink_package_valid_length(void *psRecvdat, uint32_t recvlen);

#endif
