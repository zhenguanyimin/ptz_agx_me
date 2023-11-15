
#ifndef _ALINK_SOCKET_H_
#define _ALINK_SOCKET_H_

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

int32_t alink_socket_init( alink_socket_t *psSocket, alink_order_t *psOrder );

int32_t alink_socket_connect( alink_socket_t *psSocket, alink_port_t *psPort );

#ifdef __cplusplus
}
#endif

#endif
