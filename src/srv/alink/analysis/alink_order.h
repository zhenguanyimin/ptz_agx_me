
#ifndef _ALINK_COMMAND_H_
#define _ALINK_COMMAND_H_

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "../alink_defines.h"

#ifdef __cplusplus
extern "C"
{
#endif

int32_t alink_order_init( alink_order_t *psOrder, uint8_t *pbyBuffer, uint16_t wBufferSize );

int32_t alink_order_task( alink_order_t *psOrder, alink_order_import_t *psImport );

int32_t alink_order_register_cmd( alink_order_t *psOrder, alink_cmd_list_t *psOrderList );

#ifdef __cplusplus
}
#endif

#endif
